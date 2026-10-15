// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "LessonEndAlert.h"

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJSEngine>
#include <QLocale>
#include <QSaveFile>
#include <QSettings>
#include <QSoundEffect>
#include <QStandardPaths>

#include <algorithm>
#include <cmath>
#include <utility>

#include "DataService.h"

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

LessonEndAlert *s_instance = nullptr;
constexpr qint64 kMaxFileBytes = 64 * 1024 * 1024;

//! Path QFile can open: local files, Android content:// URIs
QString openablePath(const QUrl &url)
{
    if (url.isLocalFile())
        return url.toLocalFile();
    if (url.scheme().isEmpty())
        return url.path();
    return url.toString();
}

//! "ding.mp3" also for Android documents (content://…/document/primary%3AMusic%2Fding.mp3)
QString displayName(const QUrl &url, const QString &path)
{
    QString name = QFileInfo(path).fileName();   // Android: the document's display name
    if (name.isEmpty() || name.contains(u':'))
        name = url.fileName(QUrl::FullyDecoded);
    const qsizetype cut = std::max(name.lastIndexOf(u'/'), name.lastIndexOf(u':'));
    return cut >= 0 ? name.mid(cut + 1) : name;
}

// "lessonEnd" was written by an earlier test build with its defaults (off, 5 minutes)
const QString kGroup = u"lessonAlert"_s;

QVariant stored(const QString &key)
{
    QSettings s;
    s.beginGroup(kGroup);
    return s.value(key);
}

void store(const QString &key, const QVariant &value)
{
    QSettings s;
    s.beginGroup(kGroup);
    if (value.isValid())
        s.setValue(key, value);
    else
        s.remove(key);
}

qint16 toSample(double value)
{
    return qint16(std::clamp(std::lround(value), -32768L, 32767L));
}

} // namespace

LessonEndAlert::LessonEndAlert(DataService *data, QObject *parent)
    : QObject(parent)
    , m_data(data)
{
    Q_ASSERT_X(!s_instance, "LessonEndAlert", "only one instance allowed");
    s_instance = this;
    QSettings().remove(u"lessonEnd"_s);

    m_apply.setSingleShot(true);
    m_apply.setInterval(400);
    connect(&m_apply, &QTimer::timeout, this, &LessonEndAlert::apply);
    m_tick.setInterval(10000);   // as in Qupil 1.x
    connect(&m_tick, &QTimer::timeout, this, &LessonEndAlert::check);
    if (m_data)
        connect(m_data, &DataService::changed, this, &LessonEndAlert::reschedule);

    if (background()) {
        SystemAlerts::setUp();
        connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
            if (state != Qt::ApplicationActive)
                return;
            // Back from the system settings or the permission question; iOS keeps
            // only the next 60 alerts, so the list moves on
            refreshState();
            m_force = true;
            reschedule();
        });
        refreshState();
    }
    m_notified = { enabled(), minutes(), volume() };
    reschedule();
}

LessonEndAlert::~LessonEndAlert()
{
    if (s_instance == this)
        s_instance = nullptr;
}

LessonEndAlert *LessonEndAlert::instance()
{
    return s_instance;
}

LessonEndAlert *LessonEndAlert::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "LessonEndAlert", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- settings
// Per device; without own values the ones taken over from Qupil 1.x (or its defaults)

bool LessonEndAlert::enabled() const
{
    const QVariant v = stored(u"enabled"_s);
    if (v.isValid())
        return v.toBool();
    return m_data ? m_data->setting(u"lesson_end_message"_s, 1).toInt() != 0 : true;
}

void LessonEndAlert::setEnabled(bool enabled)
{
    store(u"enabled"_s, enabled);
    notifyIfChanged();
    reschedule();
}

int LessonEndAlert::minutes() const
{
    const QVariant v = stored(u"minutes"_s);
    const int m = v.isValid() ? v.toInt() : m_data ? m_data->setting(u"lesson_end_minutes"_s, 3).toInt() : 3;
    return std::clamp(m, 1, 30);
}

void LessonEndAlert::setMinutes(int minutes)
{
    store(u"minutes"_s, std::clamp(minutes, 1, 30));
    notifyIfChanged();
    reschedule();
}

int LessonEndAlert::volume() const
{
    const QVariant v = stored(u"volume"_s);
    const int volume = v.isValid() ? v.toInt() : m_data ? m_data->setting(u"lesson_end_volume"_s, 7).toInt() : 7;
    return std::clamp(volume, 1, 10);
}

void LessonEndAlert::setVolume(int volume)
{
    store(u"volume"_s, std::clamp(volume, 1, 10));
    notifyIfChanged();
    reschedule();
}

QString LessonEndAlert::soundName() const
{
    if (!QFileInfo::exists(QDir(soundsFolder()).filePath(u"custom.wav"_s)))
        return {};
    const QString name = stored(u"customName"_s).toString();
    return name.isEmpty() ? u"custom.wav"_s : name;
}

bool LessonEndAlert::background() const
{
    return SystemAlerts::available();
}

LessonEndAlert::Permission LessonEndAlert::permission() const
{
    switch (m_state.permission) {
    case SystemAlerts::Permission::NotNeeded:
        return NotNeeded;
    case SystemAlerts::Permission::NotAsked:
        return NotAsked;
    case SystemAlerts::Permission::Granted:
        return Granted;
    case SystemAlerts::Permission::Denied:
        return Denied;
    }
    return NotNeeded;
}

void LessonEndAlert::notifyIfChanged()
{
    const QVariantList now { enabled(), minutes(), volume() };
    if (now != m_notified) {
        m_notified = now;
        emit settingsChanged();
    }
}

// ---------------------------------------------------------------------------- sound

QString LessonEndAlert::soundsFolder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + u"/sounds"_s;
}

QByteArray LessonEndAlert::sourceSound() const
{
    if (!soundName().isEmpty()) {
        QFile custom(QDir(soundsFolder()).filePath(u"custom.wav"_s));
        if (custom.open(QIODevice::ReadOnly))
            return custom.readAll();
    }
    // "ahem" from Qupil 1.x
    QFile builtIn(u":/qt/qml/Qupil/assets/sounds/lesson-end.wav"_s);
    return builtIn.open(QIODevice::ReadOnly) ? builtIn.readAll() : QByteArray();
}

QString LessonEndAlert::soundFile()
{
    const QByteArray source = sourceSound();
    const auto sound = Wav::read(source);
    if (!sound)
        return {};
    // A new name for every sound and volume: iOS keeps notification sounds by name
    const QByteArray key = QCryptographicHash::hash(source + QByteArray::number(volume()), QCryptographicHash::Md5)
                               .toHex()
                               .left(12);
    const QDir dir(soundsFolder());
    const QString name = u"lesson-end-%1.wav"_s.arg(QString::fromLatin1(key));
    const QString path = dir.filePath(name);
    if (QFileInfo::exists(path))
        return path;
    QDir().mkpath(dir.path());
    QSaveFile out(path);
    if (!out.open(QIODevice::WriteOnly))
        return {};
    out.write(Wav::write(Wav::scaled(*sound, volume() / 10.0)));
    if (!out.commit())
        return {};
    for (const QString &old : dir.entryList({ u"lesson-end-*.wav"_s }, QDir::Files)) {
        if (old != name)
            QFile::remove(dir.filePath(old));
    }
    return path;
}

void LessonEndAlert::test()
{
    play();
}

void LessonEndAlert::play()
{
    const QString file = soundFile();
    if (file.isEmpty())
        return;
    if (!m_effect) {
        m_effect = new QSoundEffect(this);
        connect(m_effect, &QSoundEffect::statusChanged, this, [this] {
            if (m_effect->status() == QSoundEffect::Error) {
                m_playWhenReady = false;
            } else if (m_playWhenReady && m_effect->status() == QSoundEffect::Ready) {
                m_playWhenReady = false;
                m_effect->play();
            }
        });
    }
    const QUrl url = QUrl::fromLocalFile(file);
    if (m_effect->source() != url)
        m_effect->setSource(url);
    if (m_effect->status() == QSoundEffect::Ready) {
        m_effect->stop();
        m_effect->play();
    } else {
        m_playWhenReady = true;   // plays when loaded
    }
}

void LessonEndAlert::useDefaultSound()
{
    QFile::remove(QDir(soundsFolder()).filePath(u"custom.wav"_s));
    store(u"customName"_s, {});
    emit settingsChanged();
    reschedule();
    play();
}

void LessonEndAlert::chooseSound(const QUrl &url)
{
    if (m_busy || url.isEmpty())
        return;
    const QString path = openablePath(url);
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly) || in.size() > kMaxFileBytes) {
        emit message(qtTrId("settings.sound.error"));
        return;
    }
    const QByteArray bytes = in.read(kMaxFileBytes);
    in.close();
    const QString name = displayName(url, path);

    if (const auto wav = Wav::read(bytes)) {
        storeCustomSound(*wav, name);
        return;
    }

    // Other formats: the system decodes them (FFmpeg, AVFoundation, Android's MediaCodec).
    // A local copy, so that the decoder needs no access to the picked document
    const QString cache = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cache);
    const QString suffix = QFileInfo(name).suffix().toLower();
    m_decodingFile = QDir(cache).filePath(u"sound-import."_s + (suffix.isEmpty() ? u"audio"_s : suffix));
    QFile copy(m_decodingFile);
    if (!copy.open(QIODevice::WriteOnly | QIODevice::Truncate) || copy.write(bytes) != bytes.size()) {
        emit message(qtTrId("settings.sound.error"));
        return;
    }
    copy.close();

    m_decoding = {};
    m_decodingName = name;
    auto *decoder = new QAudioDecoder(this);
    m_decoder = decoder;
    connect(decoder, &QAudioDecoder::bufferReady, this, [this, decoder] {
        while (m_decoder == decoder && decoder->bufferAvailable())
            decoded(decoder->read());
    });
    connect(decoder, &QAudioDecoder::finished, this, [this] { finishDecoding(true); });
    connect(decoder, qOverload<QAudioDecoder::Error>(&QAudioDecoder::error), this, [this] { finishDecoding(false); });
    QTimer::singleShot(30000, decoder, [this] { finishDecoding(false); });
    setBusy(true);
    decoder->setSource(QUrl::fromLocalFile(m_decodingFile));
    decoder->start();
}

void LessonEndAlert::decoded(const QAudioBuffer &buffer)
{
    const QAudioFormat format = buffer.format();
    const int channels = format.channelCount();
    if (!buffer.isValid() || channels <= 0 || format.sampleRate() <= 0)
        return;
    if (m_decoding.sampleRate == 0) {
        m_decoding.sampleRate = format.sampleRate();
        m_decoding.channels = std::min(channels, 2);
    }
    if (format.sampleRate() != m_decoding.sampleRate)
        return;

    const qsizetype frames = buffer.frameCount();
    const int out = m_decoding.channels;
    for (qsizetype i = 0; i < frames; ++i) {
        for (int c = 0; c < out; ++c) {
            // Mono files fill both channels of a stereo start
            const qsizetype index = i * channels + std::min(c, channels - 1);
            qint16 v = 0;
            switch (format.sampleFormat()) {
            case QAudioFormat::UInt8:
                v = qint16((int(buffer.constData<quint8>()[index]) - 128) * 256);
                break;
            case QAudioFormat::Int16:
                v = buffer.constData<qint16>()[index];
                break;
            case QAudioFormat::Int32:
                v = qint16(buffer.constData<qint32>()[index] >> 16);
                break;
            case QAudioFormat::Float:
                v = toSample(double(buffer.constData<float>()[index]) * 32767.0);
                break;
            default:
                finishDecoding(false);
                return;
            }
            m_decoding.samples.append(v);
        }
    }
    if (m_decoding.seconds() >= kMaxSeconds)
        finishDecoding(true);   // enough: the rest is cut anyway
}

void LessonEndAlert::finishDecoding(bool ok)
{
    if (!m_decoder)
        return;
    QAudioDecoder *decoder = m_decoder;
    m_decoder.clear();
    decoder->disconnect(this);
    decoder->stop();
    decoder->deleteLater();
    QFile::remove(m_decodingFile);
    setBusy(false);
    const Wav::Sound sound = std::exchange(m_decoding, {});
    // An error after a good part (some decoders complain at the very end) still counts
    if ((ok || sound.seconds() >= 0.2) && sound.isValid())
        storeCustomSound(sound, m_decodingName);
    else
        emit message(qtTrId("settings.sound.error"));
}

void LessonEndAlert::storeCustomSound(const Wav::Sound &sound, const QString &name)
{
    const Wav::Sound cut = Wav::trimmed(sound, kMaxSeconds);
    if (!cut.isValid() || cut.peak() == 0) {
        emit message(qtTrId("settings.sound.error"));
        return;
    }
    QDir().mkpath(soundsFolder());
    QSaveFile out(QDir(soundsFolder()).filePath(u"custom.wav"_s));
    if (!out.open(QIODevice::WriteOnly) || out.write(Wav::write(cut)) < 0 || !out.commit()) {
        emit message(qtTrId("settings.sound.error"));
        return;
    }
    store(u"customName"_s, name);
    emit settingsChanged();
    reschedule();
    play();   // so you hear what you chose
}

void LessonEndAlert::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

// ---------------------------------------------------------------------------- schedule

void LessonEndAlert::reschedule()
{
    m_apply.start();
}

void LessonEndAlert::apply()
{
    notifyIfChanged();
    m_alerts.clear();
    if (enabled() && m_data) {
        QList<LessonAlerts::Lesson> lessons;
        for (int day = 0; day < 7; ++day) {
            const QVariantList list = m_data->lessonsOn(day);
            for (const QVariant &v : list) {
                const QVariantMap lesson = v.toMap();
                QString name = lesson.value(u"name"_s).toString();
                if (name.isEmpty())
                    name = lesson.value(u"studentNames"_s).toString();
                lessons.append({ day, lesson.value(u"end_time"_s).toString(), name });
            }
        }
        m_alerts = LessonAlerts::weekly(lessons, minutes());
    }

    if (background()) {
        const QList<SystemAlerts::Alert> alerts = systemAlerts();
        const QString sound = alerts.isEmpty() ? QString() : soundFile();
        const QString channel = qtTrId("notify.channel.lesson_end");
        // Only when something changed (every edit of the data comes here)
        QByteArray signature = sound.toUtf8() + '|' + channel.toUtf8();
        for (const LessonAlerts::Alert &a : std::as_const(m_alerts))
            signature += '|' + QByteArray::number(a.at) + ':' + a.names.join(u',').toUtf8();
        if (!alerts.isEmpty())
            signature += '|' + alerts.first().title.toUtf8();
        if (m_force || signature != m_scheduled) {
            m_force = false;
            m_scheduled = signature;
            if (alerts.isEmpty())
                SystemAlerts::cancel();
            else
                SystemAlerts::schedule(alerts, sound, channel);
        }
        askOnce();
    } else if (m_alerts.isEmpty()) {
        m_tick.stop();
    } else if (!m_tick.isActive()) {
        m_lastCheck = QDateTime::currentDateTime();
        m_tick.start();
    }
}

QList<SystemAlerts::Alert> LessonEndAlert::systemAlerts() const
{
    QList<SystemAlerts::Alert> out;
    const QString title = qtTrId("notify.lesson_end", minutes());
    for (const LessonAlerts::Alert &a : LessonAlerts::upcoming(m_alerts, QDateTime::currentDateTime())) {
        QString text = a.names.join(u", "_s);
        if (text.isEmpty()) {
            const int end = (a.at + a.lead) % LessonAlerts::kMinutesPerDay;
            text = QLocale().toString(QTime(end / 60, end % 60), QLocale::ShortFormat);
        }
        out.append({ a.at, a.lead, title, text });
    }
    return out;
}

void LessonEndAlert::check()
{
    const QDateTime now = QDateTime::currentDateTime();
    QDateTime after = m_lastCheck.isValid() ? m_lastCheck : now;
    // The clock was set back, or the computer slept: look back a bit, not further
    if (after > now)
        after = now.addSecs(-10);
    else if (after < now.addSecs(-3600))
        after = now.addSecs(-3600);
    m_lastCheck = now;

    const QList<LessonAlerts::Alert> due = LessonAlerts::due(m_alerts, after, now);
    if (due.isEmpty())
        return;
    QStringList names;
    for (const LessonAlerts::Alert &a : due)
        names << a.names;
    play();
    emit alert(qtTrId("notify.lesson_end", minutes()), names.join(u", "_s));
}

// ---------------------------------------------------------------------------- permission (iOS, Android)

void LessonEndAlert::refreshState()
{
    SystemAlerts::queryState(this, [this](SystemAlerts::State state) {
        const bool changed = !m_stateKnown || state.permission != m_state.permission
                             || state.exactAlarms != m_state.exactAlarms;
        m_state = state;
        m_stateKnown = true;
        if (changed)
            emit permissionChanged();
        askOnce();
    });
}

void LessonEndAlert::askOnce()
{
    // Asked once by itself, as soon as there are lessons; later only with the button
    if (!background() || !m_stateKnown || m_alerts.isEmpty() || m_state.permission != SystemAlerts::Permission::NotAsked)
        return;
    if (stored(u"asked"_s).toBool())
        return;
    store(u"asked"_s, true);
    requestPermission();
}

void LessonEndAlert::requestPermission()
{
    SystemAlerts::requestPermission(this, [this] {
        refreshState();
        m_force = true;
        reschedule();
    });
}

void LessonEndAlert::openSystemSettings(bool exactAlarms)
{
    SystemAlerts::openSettings(exactAlarms);
}
