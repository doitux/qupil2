// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "DataService.h"

#include <QCollator>
#include <QFile>
#include <QColor>
#include <QHash>
#include <QJSEngine>
#include <QJsonArray>
#include <QJSValue>
#include <QJsonDocument>
#include <QLocale>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QSet>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTime>

#include <algorithm>
#include <iterator>

#include "Clock.h"
#include "Database.h"
#include "Ids.h"
#include "Csv.h"
#include "LanguageManager.h"
#include "NoteFormat.h"
#include "Store.h"

using namespace Qt::StringLiterals;
using Qupil::newId;

namespace {

DataService *s_instance = nullptr;

//! Recital interval index (Qupil 1.x) → days; 0 = never
constexpr int kIntervalDays[] = { 0, 30, 60, 90, 120, 180, 270, 360, 540, 720 };

QString todayIso()
{
    return Qupil::Clock::today().toString(Qt::ISODate);
}

QString initials(const QString &forename, const QString &surname)
{
    QString s;
    if (!forename.isEmpty())
        s += forename.at(0).toUpper();
    if (!surname.isEmpty())
        s += surname.at(0).toUpper();
    return s.isEmpty() ? u"?"_s : s;
}

QString fullName(const QString &forename, const QString &surname)
{
    return (forename + u' ' + surname).trimmed();
}

QString listName(const QString &forename, const QString &surname)
{
    if (forename.isEmpty())
        return surname;
    if (surname.isEmpty())
        return forename;
    return surname + u", "_s + forename;
}

QLocale uiLocale()
{
    if (LanguageManager *lm = LanguageManager::instance())
        return QLocale(lm->effectiveLanguage());
    return QLocale();
}

int minutesBetween(const QString &start, const QString &end)
{
    const QTime a = QTime::fromString(start, u"hh:mm"_s);
    const QTime b = QTime::fromString(end, u"hh:mm"_s);
    return a.isValid() && b.isValid() ? a.secsTo(b) / 60 : 0;
}

} // namespace

DataService::DataService(Qupil::Store *store, QObject *parent)
    : QObject(parent)
    , m_store(store)
{
    Q_ASSERT_X(!s_instance, "DataService", "only one instance allowed");
    s_instance = this;
    connect(m_store, &Qupil::Store::dataChanged, this, &DataService::bump);
}

DataService::~DataService()
{
    if (s_instance == this)
        s_instance = nullptr;
}

DataService *DataService::instance()
{
    return s_instance;
}

DataService *DataService::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "DataService", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

void DataService::bump()
{
    ++m_revision;
    emit changed();
}

// ---------------------------------------------------------------------------- SQL helpers

QVariantList DataService::rows(const QString &sql, const QVariantList &binds) const
{
    QVariantList out;
    if (!m_store->database()->isOpen())
        return out;
    QSqlQuery q(m_store->database()->connection());
    q.prepare(sql);
    for (const QVariant &b : binds)
        q.addBindValue(b);
    if (!q.exec()) {
        qWarning("Data: %s in %s", qPrintable(q.lastError().text()), qPrintable(sql.left(120)));
        return out;
    }
    const QSqlRecord r = q.record();
    while (q.next()) {
        QVariantMap m;
        for (int i = 0; i < r.count(); ++i)
            m.insert(r.fieldName(i), q.value(i));
        out << m;
    }
    return out;
}

QVariant DataService::scalar(const QString &sql, const QVariantList &binds) const
{
    const QVariantList r = rows(sql, binds);
    return r.isEmpty() ? QVariant() : r.first().toMap().cbegin().value();
}

// ---------------------------------------------------------------------------- text helpers

QString DataService::today() const
{
    return todayIso();
}

QDateTime DataService::now() const
{
    return Qupil::Clock::now();
}

QString DataService::dayHeading(const QDateTime &when) const
{
    const QLocale locale = uiLocale();
    const QDate d = when.date();
    QString pattern;
    switch (locale.language()) {
    case QLocale::German: pattern = u"dddd, d. MMMM"_s; break;
    case QLocale::French: pattern = u"dddd d MMMM"_s; break;
    case QLocale::Spanish: pattern = u"dddd, d 'de' MMMM"_s; break;
    default: pattern = u"dddd, MMMM d"_s; break;
    }
    return locale.toString(d, pattern);
}

int DataService::ageOn(const QString &birthday, const QString &onDate) const
{
    const QDate b = QDate::fromString(birthday, Qt::ISODate);
    const QDate on = onDate.isEmpty() ? Qupil::Clock::today() : QDate::fromString(onDate, Qt::ISODate);
    if (!b.isValid() || !on.isValid() || b > on)
        return -1;
    int age = on.year() - b.year();
    if (on.month() < b.month() || (on.month() == b.month() && on.day() < b.day()))
        --age;
    return age;
}

QString DataService::formatDate(const QString &iso, bool withWeekday) const
{
    const QDate d = QDate::fromString(iso, Qt::ISODate);
    if (!d.isValid())
        return iso;
    const QLocale locale = uiLocale();
    QString pattern = locale.dateFormat(QLocale::ShortFormat);
    // Always a four-digit year: dd.MM.yyyy, MM/dd/yyyy, dd/MM/yyyy
    pattern.replace(QRegularExpression(u"y+"_s), u"yyyy"_s);
    pattern.replace(QRegularExpression(u"(?<!d)d(?!d)"_s), u"dd"_s);
    pattern.replace(QRegularExpression(u"(?<!M)M(?!M)"_s), u"MM"_s);
    QString text = locale.toString(d, pattern);
    if (withWeekday)
        text = locale.dayName(d.dayOfWeek(), QLocale::ShortFormat).remove(u'.') + u", "_s + text;
    return text;
}

QString DataService::plainText(const QString &note, int maxLength) const
{
    QString text = NoteFormat::toPlainText(note).simplified();
    if (maxLength > 0 && text.size() > maxLength)
        text = text.left(maxLength - 1).trimmed() + u'…';
    return text;
}

QString DataService::noteHtml(const QString &note, const QString &markColor) const
{
    return NoteFormat::toHtml(note, markColor);
}

// ---------------------------------------------------------------------------- settings

QVariant DataService::setting(const QString &key, const QVariant &fallback) const
{
    const QVariantList r = rows(u"SELECT value FROM setting WHERE id = ? AND deleted = 0"_s, { key });
    if (r.isEmpty())
        return fallback;
    const QString v = r.first().toMap().value(u"value"_s).toString();
    if (fallback.typeId() == QMetaType::Int || fallback.typeId() == QMetaType::Bool)
        return fallback.typeId() == QMetaType::Bool ? QVariant(v.toInt() != 0) : QVariant(v.toInt());
    return v;
}

bool DataService::setSetting(const QString &key, const QVariant &given)
{
    // A list from QML can arrive as a JavaScript array (QJSValue), e.g. through a var signal parameter
    const QVariant value = given.metaType() == QMetaType::fromType<QJSValue>() ? given.value<QJSValue>().toVariant() : given;
    QString text;
    if (value.typeId() == QMetaType::QStringList || value.typeId() == QMetaType::QVariantList)
        text = QString::fromUtf8(QJsonDocument(QJsonArray::fromStringList(value.toStringList())).toJson(QJsonDocument::Compact));
    else if (value.typeId() == QMetaType::Bool)
        text = value.toBool() ? u"1"_s : u"0"_s;
    else
        text = value.toString();
    if (m_store->get(u"setting"_s, key).isEmpty())
        return !m_store->create(u"setting"_s, { { u"id"_s, key }, { u"value"_s, text } }).isEmpty();
    return m_store->update(u"setting"_s, key, { { u"value"_s, text }, { u"deleted"_s, 0 } });
}

QStringList DataService::listSetting(const QString &key) const
{
    const QString json = setting(key, QString()).toString();
    QStringList out;
    for (const QJsonValue &v : QJsonDocument::fromJson(json.toUtf8()).array())
        if (!v.toString().trimmed().isEmpty())
            out << v.toString();
    return out;
}

QStringList DataService::instrumentChoices() const
{
    QStringList out;
    for (const QVariant &r : rows(u"SELECT name FROM instrument WHERE deleted = 0 ORDER BY position, name"_s))
        out << r.toMap().value(u"name"_s).toString();
    return out;
}

bool DataService::setInstrumentChoices(const QStringList &names)
{
    const QVariantList existing = rows(u"SELECT id, name FROM instrument WHERE deleted = 0"_s);
    m_store->beginBatch();
    bool ok = true;
    QSet<QString> kept;
    for (int i = 0; i < names.size(); ++i) {
        const QString name = names.at(i).trimmed();
        if (name.isEmpty())
            continue;
        QString id;
        for (const QVariant &v : existing)
            if (v.toMap().value(u"name"_s).toString() == name && !kept.contains(v.toMap().value(u"id"_s).toString()))
                id = v.toMap().value(u"id"_s).toString();
        if (id.isEmpty())
            id = m_store->create(u"instrument"_s, { { u"name"_s, name }, { u"position"_s, i } });
        else
            ok = ok && m_store->update(u"instrument"_s, id, { { u"position"_s, i } });
        kept.insert(id);
    }
    for (const QVariant &v : existing) {
        const QString id = v.toMap().value(u"id"_s).toString();
        if (!kept.contains(id))
            ok = ok && m_store->remove(u"instrument"_s, id);
    }
    return m_store->endBatch(ok) && ok;
}

QStringList DataService::instrumentNames() const
{
    QStringList out;
    for (const QVariant &r : rows(u"SELECT name FROM instrument WHERE deleted = 0 ORDER BY position, name"_s))
        out << r.toMap().value(u"name"_s).toString();
    // Instruments that students play but that are not in the list (yet)
    for (const QVariant &r : rows(u"SELECT DISTINCT instrument FROM student WHERE deleted = 0 AND instrument <> ''"_s)) {
        const QString name = r.toMap().value(u"instrument"_s).toString();
        if (!out.contains(name))
            out << name;
    }
    return out;
}

void DataService::ensureDefaults()
{
    if (!m_store->database()->isOpen())
        return;
    // A device that waits for its first sync gets the lists of the other devices
    if (m_store->database()->meta(u"awaiting_sync"_s) == u"1"_s)
        return;
    m_store->beginBatch();
    if (setting(u"genres"_s).isNull()) {
        setSetting(u"genres"_s, QStringList {
            qtTrId("defaults.genre.song"), qtTrId("defaults.genre.baroque"), qtTrId("defaults.genre.classical"),
            qtTrId("defaults.genre.romantic"), qtTrId("defaults.genre.modern"), qtTrId("defaults.genre.new_music"),
            qtTrId("defaults.genre.musical"), qtTrId("defaults.genre.jazz"), qtTrId("defaults.genre.rock"),
            qtTrId("defaults.genre.pop"), qtTrId("defaults.genre.irish_folk") });
    }
    if (setting(u"locations"_s).isNull()) {
        setSetting(u"locations"_s, QStringList { qtTrId("defaults.location.room20"), qtTrId("defaults.location.room16"),
                                                 qtTrId("defaults.location.auditorium") });
    }
    if (setting(u"instrument_sizes"_s).isNull())
        setSetting(u"instrument_sizes"_s, QStringList { u"4/4"_s, u"3/4"_s, u"1/2"_s, u"1/4"_s, u"1/8"_s, u"1/16"_s });
    if (scalar(u"SELECT COUNT(*) FROM instrument WHERE deleted = 0"_s).toInt() == 0) {
        int position = 0;
        for (const char *id : { "defaults.instrument.violin", "defaults.instrument.viola", "defaults.instrument.cello",
                                "defaults.instrument.double_bass" })
            m_store->create(u"instrument"_s, { { u"name"_s, qtTrId(id) }, { u"position"_s, position++ } });
    }
    m_store->endBatch();
}

// ---------------------------------------------------------------------------- generic

QVariantMap DataService::get(const QString &entity, const QString &id) const
{
    const QVariantMap m = m_store->get(entity, id);
    return m.value(u"deleted"_s).toInt() != 0 ? QVariantMap() : m;
}

QVariantMap DataService::membershipInfo(const QString &membershipId) const
{
    QVariantMap m = m_store->get(u"membership"_s, membershipId);
    if (m.isEmpty())
        return {};
    const QVariantMap lesson = m_store->get(u"lesson"_s, m.value(u"lesson_id"_s).toString());
    m.insert(u"lessonName"_s, lesson.value(u"name"_s));
    m.insert(u"kind"_s, lesson.value(u"kind"_s, 1));
    const QVariantList members = activeMembers(m.value(u"lesson_id"_s).toString());
    m.insert(u"students"_s, members);
    m.insert(u"groupSize"_s, std::max<qsizetype>(1, members.size()));
    return m;
}

QString DataService::create(const QString &entity, const QVariantMap &fields)
{
    return m_store->create(entity, fields);
}

bool DataService::update(const QString &entity, const QString &id, const QVariantMap &fields)
{
    return m_store->update(entity, id, fields);
}

bool DataService::remove(const QString &entity, const QString &id)
{
    return m_store->remove(entity, id);
}

// ---------------------------------------------------------------------------- lessons

QVariantList DataService::activeMembers(const QString &lessonId) const
{
    QVariantList out;
    const QVariantList r = rows(
        u"SELECT m.id AS membership_id, s.id, s.forename, s.surname, s.instrument, s.instrument_size, s.birthday "
        u"FROM membership m JOIN student s ON s.id = m.student_id "
        u"WHERE m.deleted = 0 AND s.deleted = 0 AND m.lesson_id = ? AND (m.end_date = '' OR m.end_date > ?) "
        u"ORDER BY s.surname COLLATE NOCASE, s.forename COLLATE NOCASE"_s,
        { lessonId, todayIso() });
    for (const QVariant &v : r) {
        QVariantMap m = v.toMap();
        const QString forename = m.value(u"forename"_s).toString();
        const QString surname = m.value(u"surname"_s).toString();
        m.insert(u"name"_s, fullName(forename, surname));
        m.insert(u"listName"_s, listName(forename, surname));
        m.insert(u"initials"_s, initials(forename, surname));
        m.insert(u"age"_s, ageOn(m.value(u"birthday"_s).toString()));
        out << m;
    }
    return out;
}

QVariantMap DataService::lessonSummary(const QVariantMap &row, bool withDetails) const
{
    QVariantMap m = row;
    const QString id = row.value(u"id"_s).toString();
    const QVariantList members = activeMembers(id);
    QStringList names;
    for (const QVariant &s : members)
        names << s.toMap().value(u"name"_s).toString();
    m.insert(u"students"_s, members);
    m.insert(u"studentNames"_s, names.join(u", "_s));
    m.insert(u"studentCount"_s, members.size());
    m.insert(u"minutes"_s, minutesBetween(row.value(u"start_time"_s).toString(), row.value(u"end_time"_s).toString()));

    // Reminders for every lesson (mode 1) or for one of its students (mode 2)
    int reminderCount = scalar(u"SELECT COUNT(*) FROM reminder WHERE deleted = 0 AND mode = 1"_s).toInt();
    for (const QVariant &s : members)
        reminderCount += scalar(u"SELECT COUNT(*) FROM reminder WHERE deleted = 0 AND mode = 2 AND student_id = ?"_s,
                                { s.toMap().value(u"id"_s) }).toInt();
    m.insert(u"reminderCount"_s, reminderCount);

    if (withDetails) {
        QVariantList detailed;
        for (const QVariant &s : members) {
            QVariantMap st = s.toMap();
            const QString membership = st.value(u"membership_id"_s).toString();
            st.insert(u"inProgress"_s, scalar(u"SELECT COUNT(*) FROM piece WHERE deleted = 0 AND membership_id = ? AND state = 1"_s, { membership }));
            st.insert(u"ready"_s, scalar(u"SELECT COUNT(*) FROM piece WHERE deleted = 0 AND membership_id = ? AND state = 3"_s, { membership }));
            const QVariantList last = rows(u"SELECT date, content FROM note WHERE deleted = 0 AND membership_id = ? "
                                           u"ORDER BY date DESC LIMIT 1"_s, { membership });
            if (!last.isEmpty()) {
                const QVariantMap n = last.first().toMap();
                st.insert(u"lastNoteDate"_s, n.value(u"date"_s));
                st.insert(u"lastNote"_s, plainText(n.value(u"content"_s).toString(), 90));
            }
            detailed << st;
        }
        m.insert(u"students"_s, detailed);
        QVariantList reminders;
        for (const QVariant &r : this->reminders()) {
            const QVariantMap rm = r.toMap();
            const int mode = rm.value(u"mode"_s).toInt();
            if (mode == 1)
                reminders << rm;
            else if (mode == 2) {
                for (const QVariant &s : members)
                    if (s.toMap().value(u"id"_s) == rm.value(u"student_id"_s))
                        reminders << rm;
            }
        }
        m.insert(u"reminders"_s, reminders);
    }
    return m;
}

QVariantList DataService::lessonsOn(int weekday) const
{
    QVariantList out;
    for (const QVariant &r : rows(u"SELECT * FROM lesson WHERE deleted = 0 AND active = 1 AND irregular = 0 AND weekday = ? "
                                  u"ORDER BY start_time, name"_s, { weekday }))
        out << lessonSummary(r.toMap(), true);
    return out;
}

QVariantList DataService::irregularLessons() const
{
    QVariantList out;
    for (const QVariant &r : rows(u"SELECT * FROM lesson WHERE deleted = 0 AND active = 1 AND irregular = 1 "
                                  u"ORDER BY name COLLATE NOCASE"_s))
        out << lessonSummary(r.toMap(), true);
    return out;
}

QVariantList DataService::lessonCountPerWeekday() const
{
    QVariantList counts { 0, 0, 0, 0, 0, 0, 0 };
    for (const QVariant &r : rows(u"SELECT weekday, COUNT(*) AS n FROM lesson WHERE deleted = 0 AND active = 1 "
                                  u"AND irregular = 0 GROUP BY weekday"_s)) {
        const QVariantMap m = r.toMap();
        const int day = m.value(u"weekday"_s).toInt();
        if (day >= 0 && day < 7)
            counts[day] = m.value(u"n"_s).toInt();
    }
    return counts;
}

QVariantMap DataService::lesson(const QString &id) const
{
    const QVariantList r = rows(u"SELECT * FROM lesson WHERE id = ? AND deleted = 0"_s, { id });
    return r.isEmpty() ? QVariantMap() : lessonSummary(r.first().toMap(), true);
}

QString DataService::autoLessonName(int kind, bool irregular, const QString &start, const QString &end,
                                    const QString &location, const QStringList &studentIds) const
{
    QString name = kind == 2 ? qtTrId("lesson.prefix.group")
                 : kind == 3 ? qtTrId("lesson.prefix.ensemble")
                             : qtTrId("lesson.prefix.individual");
    if (!irregular) {
        const int minutes = minutesBetween(start, end);
        if (minutes > 0)
            name += QString::number(minutes) + u'-';
    }
    if (!location.isEmpty())
        name += location.left(3) + u'-';
    QList<QVariantMap> people;
    for (const QString &id : studentIds) {
        const QVariantMap s = m_store->get(u"student"_s, id);
        if (!s.isEmpty())
            people << s;
    }
    std::sort(people.begin(), people.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return listName(a.value(u"forename"_s).toString(), a.value(u"surname"_s).toString()).localeAwareCompare(
                   listName(b.value(u"forename"_s).toString(), b.value(u"surname"_s).toString())) < 0;
    });
    if (kind == 1 && !people.isEmpty()) {
        name += people.first().value(u"forename"_s).toString().left(3) + people.first().value(u"surname"_s).toString().left(3);
    } else {
        for (const QVariantMap &p : people)
            name += p.value(u"forename"_s).toString().left(1);
    }
    return name;
}

bool DataService::endMembership(const QString &membershipId, const QString &lessonName)
{
    const QVariantMap m = m_store->get(u"membership"_s, membershipId);
    const QDate start = QDate::fromString(m.value(u"start_date"_s).toString(), Qt::ISODate);
    // Added by mistake (less than 30 days): remove; otherwise keep the history under the old name
    if (!start.isValid() || start.daysTo(Qupil::Clock::today()) <= 30) {
        const bool hasContent = scalar(u"SELECT (SELECT COUNT(*) FROM note WHERE deleted = 0 AND membership_id = ?) + "
                                       u"(SELECT COUNT(*) FROM piece WHERE deleted = 0 AND membership_id = ?)"_s,
                                       { membershipId, membershipId }).toInt() > 0;
        if (!hasContent)
            return m_store->remove(u"membership"_s, membershipId);
    }
    return m_store->update(u"membership"_s, membershipId, { { u"end_date"_s, todayIso() }, { u"previous_name"_s, lessonName } });
}

QString DataService::saveLesson(const QString &id, const QVariantMap &input, const QStringList &studentIds)
{
    QVariantMap fields = input;
    const int kind = fields.value(u"kind"_s, 1).toInt();
    if (kind == 1 && studentIds.size() > 1)
        return qtTrId("lesson.individual_one_student");
    const bool irregular = fields.value(u"irregular"_s).toBool();
    fields.insert(u"irregular"_s, irregular ? 1 : 0);
    if (irregular) {
        fields.insert(u"start_time"_s, QString());
        fields.insert(u"end_time"_s, QString());
    }
    if (fields.value(u"auto_name"_s, 1).toBool()) {
        fields.insert(u"auto_name"_s, 1);
        fields.insert(u"name"_s, autoLessonName(kind, irregular, fields.value(u"start_time"_s).toString(),
                                               fields.value(u"end_time"_s).toString(),
                                               fields.value(u"location"_s).toString(), studentIds));
    } else {
        fields.insert(u"auto_name"_s, 0);
    }

    m_store->beginBatch();
    bool ok = true;
    QString lessonId = id;
    QString oldName;
    if (lessonId.isEmpty()) {
        fields.insert(u"active"_s, 1);
        lessonId = m_store->create(u"lesson"_s, fields);
        ok = !lessonId.isEmpty();
    } else {
        oldName = m_store->get(u"lesson"_s, lessonId).value(u"name"_s).toString();
        ok = m_store->update(u"lesson"_s, lessonId, fields);
    }

    // Students who left / joined
    QSet<QString> wanted(studentIds.cbegin(), studentIds.cend());
    for (const QVariant &v : activeMembers(lessonId)) {
        const QVariantMap m = v.toMap();
        const QString student = m.value(u"id"_s).toString();
        if (!wanted.remove(student) && ok)
            ok = endMembership(m.value(u"membership_id"_s).toString(), oldName.isEmpty() ? fields.value(u"name"_s).toString() : oldName);
    }
    for (const QString &student : studentIds) {
        if (!wanted.contains(student) || !ok)
            continue;
        ok = !m_store->create(u"membership"_s, { { u"lesson_id"_s, lessonId }, { u"student_id"_s, student },
                                                 { u"start_date"_s, todayIso() } }).isEmpty();
    }
    m_store->endBatch(ok);
    m_lastId = ok ? lessonId : QString();
    return ok ? QString() : m_store->lastError();
}

bool DataService::deleteLesson(const QString &id)
{
    const QString name = m_store->get(u"lesson"_s, id).value(u"name"_s).toString();
    m_store->beginBatch();
    bool ok = true;
    for (const QVariant &v : activeMembers(id))
        ok = ok && endMembership(v.toMap().value(u"membership_id"_s).toString(), name);
    // With history (ended memberships) the lesson stays as inactive record
    const int history = scalar(u"SELECT COUNT(*) FROM membership WHERE deleted = 0 AND lesson_id = ?"_s, { id }).toInt();
    ok = ok && (history > 0 ? m_store->update(u"lesson"_s, id, { { u"active"_s, 0 } }) : m_store->remove(u"lesson"_s, id));
    return m_store->endBatch(ok) && ok;
}

// ---------------------------------------------------------------------------- students

QVariantList DataService::students(const QString &filter) const
{
    // Active lessons per student
    QHash<QString, QStringList> lessons;
    for (const QVariant &v : rows(u"SELECT m.student_id, l.name FROM membership m JOIN lesson l ON l.id = m.lesson_id "
                                  u"WHERE m.deleted = 0 AND l.deleted = 0 AND l.active = 1 AND (m.end_date = '' OR m.end_date > ?) "
                                  u"ORDER BY l.weekday, l.start_time"_s, { todayIso() })) {
        const QVariantMap m = v.toMap();
        lessons[m.value(u"student_id"_s).toString()] << m.value(u"name"_s).toString();
    }

    const QString needle = filter.trimmed().toLower();
    QList<QVariantMap> list;
    for (const QVariant &v : rows(u"SELECT id, forename, surname, instrument, instrument_size, birthday FROM student "
                                  u"WHERE deleted = 0"_s)) {
        QVariantMap m = v.toMap();
        const QString forename = m.value(u"forename"_s).toString();
        const QString surname = m.value(u"surname"_s).toString();
        const QString instrument = m.value(u"instrument"_s).toString();
        if (!needle.isEmpty() && !(forename + u' ' + surname + u' ' + surname + u' ' + instrument).toLower().contains(needle))
            continue;
        const QStringList ls = lessons.value(m.value(u"id"_s).toString());
        m.insert(u"name"_s, fullName(forename, surname));
        m.insert(u"listName"_s, listName(forename, surname));
        m.insert(u"initials"_s, initials(forename, surname));
        m.insert(u"age"_s, ageOn(m.value(u"birthday"_s).toString()));
        m.insert(u"lessons"_s, ls.join(u" · "_s));
        m.insert(u"active"_s, !ls.isEmpty());
        list << m;
    }
    QCollator collator(uiLocale());
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    std::sort(list.begin(), list.end(), [&collator](const QVariantMap &a, const QVariantMap &b) {
        return collator.compare(a.value(u"listName"_s).toString(), b.value(u"listName"_s).toString()) < 0;
    });
    QVariantList out;
    for (const QVariantMap &m : list) {
        QVariantMap e = m;
        // Section letter for the list (umlauts sort with their base letter)
        const QString base = m.value(u"listName"_s).toString().normalized(QString::NormalizationForm_D).left(1).toUpper();
        e.insert(u"section"_s, base.isEmpty() ? u"#"_s : base);
        out << e;
    }
    return out;
}

QVariantMap DataService::student(const QString &id) const
{
    QVariantMap m = m_store->get(u"student"_s, id);
    if (m.isEmpty() || m.value(u"deleted"_s).toInt() != 0)
        return {};
    const QString forename = m.value(u"forename"_s).toString();
    const QString surname = m.value(u"surname"_s).toString();
    m.insert(u"name"_s, fullName(forename, surname));
    m.insert(u"listName"_s, listName(forename, surname));
    m.insert(u"initials"_s, initials(forename, surname));
    m.insert(u"age"_s, ageOn(m.value(u"birthday"_s).toString()));
    const QDate first = QDate::fromString(m.value(u"first_lesson"_s).toString(), Qt::ISODate);
    m.insert(u"since"_s, first.isValid() ? first.year() : 0);

    QVariantList memberships;
    for (const QVariant &v : rows(u"SELECT m.id, m.lesson_id, m.start_date, m.end_date, m.previous_name, "
                                  u"l.name AS lesson_name, l.kind, l.active AS lesson_active "
                                  u"FROM membership m LEFT JOIN lesson l ON l.id = m.lesson_id "
                                  u"WHERE m.deleted = 0 AND m.student_id = ? "
                                  u"ORDER BY (m.end_date = '' OR m.end_date > ?) DESC, m.start_date DESC"_s,
                                  { id, todayIso() })) {
        QVariantMap ms = v.toMap();
        const QString end = ms.value(u"end_date"_s).toString();
        const bool active = end.isEmpty() || end > todayIso();
        QString name = ms.value(u"lesson_name"_s).toString();
        if (!active && !ms.value(u"previous_name"_s).toString().isEmpty())
            name = ms.value(u"previous_name"_s).toString();
        ms.insert(u"active"_s, active);
        ms.insert(u"label"_s, active ? name : name + u' ' + qtTrId("lesson.inactive_suffix"));
        ms.insert(u"name"_s, name);
        memberships << ms;
    }
    m.insert(u"memberships"_s, memberships);
    m.insert(u"activeLessons"_s, std::count_if(memberships.cbegin(), memberships.cend(),
                                               [](const QVariant &v) { return v.toMap().value(u"active"_s).toBool(); }));
    return m;
}

QString DataService::createStudent(const QVariantMap &fields)
{
    QVariantMap f = fields;
    if (!f.contains(u"first_lesson"_s))
        f.insert(u"first_lesson"_s, todayIso());
    return m_store->create(u"student"_s, f);
}

bool DataService::updateStudent(const QString &id, const QVariantMap &fields)
{
    return m_store->update(u"student"_s, id, fields);
}

bool DataService::deleteStudent(const QString &id)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QVariant &v : rows(u"SELECT id FROM membership WHERE deleted = 0 AND student_id = ?"_s, { id })) {
        const QString membership = v.toMap().value(u"id"_s).toString();
        for (const char *table : { "note", "piece" })
            for (const QVariant &c : rows(u"SELECT id FROM %1 WHERE deleted = 0 AND membership_id = ?"_s.arg(QLatin1StringView(table)), { membership }))
                ok = ok && m_store->remove(QLatin1StringView(table), c.toMap().value(u"id"_s).toString());
        ok = ok && m_store->remove(u"membership"_s, membership);
    }
    for (const QVariant &v : rows(u"SELECT id FROM activity WHERE deleted = 0 AND student_id = ?"_s, { id }))
        ok = ok && m_store->remove(u"activity"_s, v.toMap().value(u"id"_s).toString());
    for (const QVariant &v : rows(u"SELECT id FROM reminder WHERE deleted = 0 AND student_id = ? AND mode = 2"_s, { id }))
        ok = ok && m_store->remove(u"reminder"_s, v.toMap().value(u"id"_s).toString());
    for (const QVariant &v : rows(u"SELECT id FROM library_item WHERE deleted = 0 AND lent_to = ?"_s, { id }))
        ok = ok && m_store->update(u"library_item"_s, v.toMap().value(u"id"_s).toString(), { { u"lent_to"_s, QString() } });
    ok = ok && m_store->remove(u"student"_s, id);
    return m_store->endBatch(ok) && ok;
}

// ---------------------------------------------------------------------------- notes and pieces

QStringList DataService::activeMembershipsOfSameLesson(const QString &membershipId) const
{
    QStringList out;
    const QString lesson = m_store->get(u"membership"_s, membershipId).value(u"lesson_id"_s).toString();
    for (const QVariant &v : activeMembers(lesson))
        out << v.toMap().value(u"membership_id"_s).toString();
    if (!out.contains(membershipId))
        out.prepend(membershipId);
    return out;
}

int DataService::groupSize(const QString &membershipId) const
{
    return activeMembershipsOfSameLesson(membershipId).size();
}

QStringList DataService::groupCopies(const QString &entity, const QString &id) const
{
    const QString key = m_store->get(entity, id).value(u"group_key"_s).toString();
    if (key.isEmpty())
        return { id };
    QStringList out;
    for (const QVariant &v : rows(u"SELECT id FROM %1 WHERE deleted = 0 AND group_key = ?"_s.arg(entity), { key }))
        out << v.toMap().value(u"id"_s).toString();
    return out.isEmpty() ? QStringList { id } : out;
}

QVariantList DataService::notes(const QString &membershipId) const
{
    QString sql = u"SELECT * FROM note WHERE deleted = 0 AND membership_id = ? ORDER BY date DESC, id DESC"_s;
    if (setting(u"limit_notes"_s, 0).toInt() != 0)
        sql += u" LIMIT %1"_s.arg(std::max(1, setting(u"limit_notes_count"_s, 30).toInt()));
    QVariantList out;
    for (const QVariant &v : rows(sql, { membershipId })) {
        QVariantMap m = v.toMap();
        const QString key = m.value(u"group_key"_s).toString();
        m.insert(u"shared"_s, key.isEmpty() ? 1 : scalar(u"SELECT COUNT(*) FROM note WHERE deleted = 0 AND group_key = ?"_s, { key }).toInt());
        out << m;
    }
    return out;
}

QString DataService::addNote(const QString &membershipId, const QString &date, const QString &content, bool forAll)
{
    const QString key = newId();
    const QStringList targets = forAll ? activeMembershipsOfSameLesson(membershipId) : QStringList { membershipId };
    m_store->beginBatch();
    QString first;
    for (const QString &m : targets) {
        const QString id = m_store->create(u"note"_s, { { u"membership_id"_s, m }, { u"group_key"_s, key },
                                                        { u"date"_s, date }, { u"content"_s, NoteFormat::normalized(content) } });
        if (m == membershipId)
            first = id;
    }
    m_store->endBatch(!first.isEmpty());
    return first;
}

bool DataService::updateNote(const QString &id, const QString &date, const QString &content, bool forAll)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QString &copy : forAll ? groupCopies(u"note"_s, id) : QStringList { id })
        ok = ok && m_store->update(u"note"_s, copy, { { u"date"_s, date }, { u"content"_s, NoteFormat::normalized(content) } });
    return m_store->endBatch(ok) && ok;
}

bool DataService::deleteNote(const QString &id, bool forAll)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QString &copy : forAll ? groupCopies(u"note"_s, id) : QStringList { id })
        ok = ok && m_store->remove(u"note"_s, copy);
    return m_store->endBatch(ok) && ok;
}

QVariantList DataService::pieces(const QString &membershipId, int stateFilter) const
{
    QString sql = u"SELECT * FROM piece WHERE deleted = 0 AND membership_id = ?"_s;
    QVariantList binds { membershipId };
    if (stateFilter >= 0) {
        sql += u" AND state = ?"_s;
        binds << stateFilter;
    }
    // Finished pieces last, newest first
    sql += u" ORDER BY (state = 4), start_date DESC, title COLLATE NOCASE"_s;
    if (setting(u"limit_pieces"_s, 0).toInt() != 0)
        sql += u" LIMIT %1"_s.arg(std::max(1, setting(u"limit_pieces_count"_s, 20).toInt()));
    QVariantList out;
    for (const QVariant &v : rows(sql, binds)) {
        QVariantMap m = v.toMap();
        const QString key = m.value(u"group_key"_s).toString();
        m.insert(u"shared"_s, key.isEmpty() ? 1 : scalar(u"SELECT COUNT(*) FROM piece WHERE deleted = 0 AND group_key = ?"_s, { key }).toInt());
        out << m;
    }
    return out;
}

QVariantList DataService::pieceCounts(const QString &membershipId) const
{
    QVariantList counts { 0, 0, 0, 0, 0, 0 };
    int all = 0;
    for (const QVariant &v : rows(u"SELECT state, COUNT(*) AS n FROM piece WHERE deleted = 0 AND membership_id = ? GROUP BY state"_s,
                                  { membershipId })) {
        const QVariantMap m = v.toMap();
        const int state = m.value(u"state"_s).toInt();
        if (state >= 0 && state < 5)
            counts[state] = m.value(u"n"_s).toInt();
        all += m.value(u"n"_s).toInt();
    }
    counts[5] = all;
    return counts;
}

QString DataService::addPiece(const QString &membershipId, const QVariantMap &fields, bool forAll)
{
    QVariantMap f = fields;
    f.insert(u"group_key"_s, newId());
    if (!f.contains(u"start_date"_s) || f.value(u"start_date"_s).toString().isEmpty())
        f.insert(u"start_date"_s, todayIso());
    if (f.value(u"state"_s).toInt() == 4 && f.value(u"end_date"_s).toString().isEmpty())
        f.insert(u"end_date"_s, todayIso());
    const QStringList targets = forAll ? activeMembershipsOfSameLesson(membershipId) : QStringList { membershipId };
    m_store->beginBatch();
    QString first;
    for (const QString &m : targets) {
        f.insert(u"membership_id"_s, m);
        const QString id = m_store->create(u"piece"_s, f);
        if (m == membershipId)
            first = id;
    }
    m_store->endBatch(!first.isEmpty());
    return first;
}

bool DataService::updatePiece(const QString &id, const QVariantMap &fields, bool forAll)
{
    QVariantMap f = fields;
    f.remove(u"membership_id"_s);
    f.remove(u"group_key"_s);
    if (f.value(u"state"_s).toInt() == 4 && f.contains(u"state"_s) && f.value(u"end_date"_s).toString().isEmpty()) {
        const QVariantMap current = m_store->get(u"piece"_s, id);
        if (current.value(u"end_date"_s).toString().isEmpty())
            f.insert(u"end_date"_s, todayIso());
    }
    m_store->beginBatch();
    bool ok = true;
    for (const QString &copy : forAll ? groupCopies(u"piece"_s, id) : QStringList { id })
        ok = ok && m_store->update(u"piece"_s, copy, f);
    return m_store->endBatch(ok) && ok;
}

bool DataService::deletePiece(const QString &id, bool forAll)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QString &copy : forAll ? groupCopies(u"piece"_s, id) : QStringList { id }) {
        ok = ok && m_store->remove(u"piece"_s, copy);
        for (const QVariant &v : rows(u"SELECT id FROM event_item WHERE deleted = 0 AND piece_id = ?"_s, { copy }))
            ok = ok && m_store->remove(u"event_item"_s, v.toMap().value(u"id"_s).toString());
    }
    return m_store->endBatch(ok) && ok;
}

QStringList DataService::composers() const
{
    QStringList out;
    for (const QVariant &v : rows(u"SELECT composer FROM piece WHERE deleted = 0 AND composer <> '' UNION "
                                  u"SELECT composer FROM external_piece WHERE deleted = 0 AND composer <> '' "
                                  u"ORDER BY 1 COLLATE NOCASE"_s))
        out << v.toMap().value(u"composer"_s).toString();
    return out;
}

QVariantList DataService::activities(const QString &studentId) const
{
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT * FROM activity WHERE deleted = 0 AND student_id = ? "
                                  u"ORDER BY continuous DESC, date DESC"_s, { studentId })) {
        QVariantMap m = v.toMap();
        const QString end = m.value(u"end_date"_s).toString();
        m.insert(u"running"_s, m.value(u"continuous"_s).toInt() == 1 && (end.isEmpty() || end > todayIso()));
        out << m;
    }
    return out;
}

QVariantList DataService::loans(const QString &studentId) const
{
    return rows(u"SELECT * FROM library_item WHERE deleted = 0 AND lent_to = ? ORDER BY lent_since DESC"_s, { studentId });
}

// ---------------------------------------------------------------------------- events

QString DataService::performers(const QString &groupKey) const
{
    // Everyone who has a copy of this piece, also students who left the lesson
    const QVariantList people = rows(
        u"SELECT DISTINCT s.forename, s.surname, s.birthday, s.instrument FROM piece p "
        u"JOIN membership m ON m.id = p.membership_id JOIN student s ON s.id = m.student_id "
        u"WHERE p.deleted = 0 AND s.deleted = 0 AND p.group_key = ? ORDER BY s.surname COLLATE NOCASE, s.forename"_s,
        { groupKey });
    QStringList names;
    QSet<QString> instruments;
    for (const QVariant &v : people) {
        const QVariantMap p = v.toMap();
        const int age = ageOn(p.value(u"birthday"_s).toString());
        QString name = listName(p.value(u"forename"_s).toString(), p.value(u"surname"_s).toString());
        if (age >= 0)
            name += u" (%1)"_s.arg(age);
        names << name;
        instruments.insert(p.value(u"instrument"_s).toString());
    }
    QString text = names.join(u" · "_s);
    if (instruments.size() == 1 && !instruments.cbegin()->isEmpty())
        text += u" – "_s + *instruments.cbegin();
    return text;
}

QVariantList DataService::events() const
{
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT * FROM event WHERE deleted = 0 ORDER BY state, date, time"_s)) {
        QVariantMap m = v.toMap();
        const QString id = m.value(u"id"_s).toString();
        m.insert(u"itemCount"_s, scalar(u"SELECT COUNT(*) FROM event_item WHERE deleted = 0 AND event_id = ?"_s, { id }));
        out << m;
    }
    return out;
}

QVariantMap DataService::event(const QString &id) const
{
    QVariantMap e = m_store->get(u"event"_s, id);
    if (e.isEmpty() || e.value(u"deleted"_s).toInt() != 0)
        return {};
    QVariantList programme;
    int playing = 0;
    int externals = 0;
    for (const QVariant &v : rows(u"SELECT * FROM event_item WHERE deleted = 0 AND event_id = ? ORDER BY position, id"_s, { id })) {
        const QVariantMap item = v.toMap();
        QVariantMap row { { u"itemId"_s, item.value(u"id"_s) } };
        const QString external = item.value(u"external_id"_s).toString();
        if (!external.isEmpty()) {
            const QVariantMap x = m_store->get(u"external_piece"_s, external);
            if (x.isEmpty() || x.value(u"deleted"_s).toInt())
                continue;
            row.insert(u"external"_s, true);
            row.insert(u"composer"_s, x.value(u"composer"_s));
            row.insert(u"title"_s, x.value(u"title"_s));
            row.insert(u"genre"_s, x.value(u"genre"_s));
            row.insert(u"duration"_s, x.value(u"duration"_s));
            row.insert(u"performers"_s, x.value(u"performer"_s).toString().split(u'\n', Qt::SkipEmptyParts).join(u" · "_s));
            ++externals;
        } else {
            const QVariantMap p = m_store->get(u"piece"_s, item.value(u"piece_id"_s).toString());
            if (p.isEmpty() || p.value(u"deleted"_s).toInt())
                continue;
            row.insert(u"external"_s, false);
            row.insert(u"pieceId"_s, p.value(u"id"_s));
            row.insert(u"composer"_s, p.value(u"composer"_s));
            row.insert(u"title"_s, p.value(u"title"_s));
            row.insert(u"genre"_s, p.value(u"genre"_s));
            row.insert(u"duration"_s, p.value(u"duration"_s));
            row.insert(u"performers"_s, performers(p.value(u"group_key"_s).toString()));
        }
        playing += row.value(u"duration"_s).toInt();
        row.insert(u"position"_s, programme.size() + 1);
        programme << row;
    }
    const int moderation = setting(u"event_moderation_minutes"_s, 4).toInt();
    const int between = setting(u"event_between_minutes"_s, 2).toInt();
    e.insert(u"programme"_s, programme);
    e.insert(u"externalCount"_s, externals);
    e.insert(u"playingMinutes"_s, playing);
    e.insert(u"moderationMinutes"_s, moderation);
    e.insert(u"betweenMinutes"_s, between);
    e.insert(u"totalMinutes"_s, programme.isEmpty() ? 0 : playing + moderation + between * int(programme.size()));
    return e;
}

QVariantList DataService::readyPieces(const QString &eventId) const
{
    QSet<QString> planned;
    for (const QVariant &v : rows(u"SELECT p.group_key FROM event_item i JOIN piece p ON p.id = i.piece_id "
                                  u"WHERE i.deleted = 0 AND i.event_id = ?"_s, { eventId }))
        planned.insert(v.toMap().value(u"group_key"_s).toString());
    QVariantList out;
    QSet<QString> seen;
    for (const QVariant &v : rows(u"SELECT p.*, l.location, m.student_id FROM piece p JOIN membership m ON m.id = p.membership_id "
                                  u"JOIN lesson l ON l.id = m.lesson_id "
                                  u"WHERE p.deleted = 0 AND m.deleted = 0 AND p.state = 3 AND (m.end_date = '' OR m.end_date > ?) "
                                  u"ORDER BY p.composer COLLATE NOCASE, p.title COLLATE NOCASE"_s, { todayIso() })) {
        QVariantMap m = v.toMap();
        const QString key = m.value(u"group_key"_s).toString();
        if (planned.contains(key) || seen.contains(key))
            continue;
        seen.insert(key);
        m.insert(u"performers"_s, performers(key));
        out << m;
    }
    return out;
}

QVariantList DataService::pieceCopies(const QString &groupKey) const
{
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT p.id AS piece_id, p.membership_id, m.student_id, s.forename, s.surname, "
                                  u"(m.end_date = '' OR m.end_date > ?) AS current FROM piece p "
                                  u"JOIN membership m ON m.id = p.membership_id JOIN student s ON s.id = m.student_id "
                                  u"WHERE p.deleted = 0 AND m.deleted = 0 AND s.deleted = 0 AND p.group_key = ? "
                                  u"ORDER BY current DESC, s.surname COLLATE NOCASE, s.forename COLLATE NOCASE"_s,
                                  { todayIso(), groupKey })) {
        QVariantMap m = v.toMap();
        m.insert(u"name"_s, fullName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        out << m;
    }
    return out;
}

QString DataService::saveEvent(const QString &id, const QVariantMap &fields)
{
    if (id.isEmpty()) {
        QVariantMap f = fields;
        f.insert(u"state"_s, 0);
        m_lastId = m_store->create(u"event"_s, f);
        return m_lastId;
    }
    m_lastId = id;
    return m_store->update(u"event"_s, id, fields) ? id : QString();
}

bool DataService::deleteEvent(const QString &id)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QVariant &v : rows(u"SELECT id, external_id FROM event_item WHERE deleted = 0 AND event_id = ?"_s, { id })) {
        const QVariantMap item = v.toMap();
        ok = ok && m_store->remove(u"event_item"_s, item.value(u"id"_s).toString());
        if (!item.value(u"external_id"_s).toString().isEmpty())
            ok = ok && m_store->remove(u"external_piece"_s, item.value(u"external_id"_s).toString());
    }
    ok = ok && m_store->remove(u"event"_s, id);
    return m_store->endBatch(ok) && ok;
}

bool DataService::addToProgramme(const QString &eventId, const QStringList &pieceIds)
{
    int position = scalar(u"SELECT COALESCE(MAX(position), -1) FROM event_item WHERE deleted = 0 AND event_id = ?"_s, { eventId }).toInt();
    m_store->beginBatch();
    bool ok = true;
    for (const QString &piece : pieceIds)
        ok = ok && !m_store->create(u"event_item"_s, { { u"event_id"_s, eventId }, { u"piece_id"_s, piece },
                                                       { u"position"_s, ++position } }).isEmpty();
    return m_store->endBatch(ok) && ok;
}

bool DataService::addExternal(const QString &eventId, const QVariantMap &fields)
{
    const int position = scalar(u"SELECT COALESCE(MAX(position), -1) FROM event_item WHERE deleted = 0 AND event_id = ?"_s, { eventId }).toInt() + 1;
    m_store->beginBatch();
    const QString external = m_store->create(u"external_piece"_s, fields);
    const bool ok = !external.isEmpty()
                    && !m_store->create(u"event_item"_s, { { u"event_id"_s, eventId }, { u"external_id"_s, external },
                                                           { u"position"_s, position } }).isEmpty();
    return m_store->endBatch(ok) && ok;
}

bool DataService::removeFromProgramme(const QString &itemId)
{
    const QVariantMap item = m_store->get(u"event_item"_s, itemId);
    m_store->beginBatch();
    bool ok = m_store->remove(u"event_item"_s, itemId);
    if (!item.value(u"external_id"_s).toString().isEmpty())
        ok = ok && m_store->remove(u"external_piece"_s, item.value(u"external_id"_s).toString());
    return m_store->endBatch(ok) && ok;
}

bool DataService::reorderProgramme(const QString &, const QStringList &itemIds)
{
    m_store->beginBatch();
    bool ok = true;
    for (int i = 0; i < itemIds.size(); ++i)
        ok = ok && m_store->update(u"event_item"_s, itemIds.at(i), { { u"position"_s, i } });
    return m_store->endBatch(ok) && ok;
}

bool DataService::setEventState(const QString &id, int state, bool addActivities, bool finishPieces)
{
    const QVariantMap e = m_store->get(u"event"_s, id);
    if (e.isEmpty())
        return false;
    m_store->beginBatch();
    bool ok = m_store->update(u"event"_s, id, { { u"state"_s, state } });
    if (state == 1 && (addActivities || finishPieces)) {
        for (const QVariant &v : rows(u"SELECT p.group_key, p.composer, p.title FROM event_item i JOIN piece p ON p.id = i.piece_id "
                                      u"WHERE i.deleted = 0 AND i.event_id = ?"_s, { id })) {
            const QVariantMap piece = v.toMap();
            const QString key = piece.value(u"group_key"_s).toString();
            const QVariantList copies = rows(
                u"SELECT p.id, m.student_id, l.kind FROM piece p JOIN membership m ON m.id = p.membership_id "
                u"LEFT JOIN lesson l ON l.id = m.lesson_id WHERE p.deleted = 0 AND p.group_key = ?"_s, { key });
            for (const QVariant &c : copies) {
                const QVariantMap copy = c.toMap();
                if (addActivities) {
                    const QString description = u"%1, %2: %3 - %4"_s.arg(e.value(u"title"_s).toString(), e.value(u"location"_s).toString(),
                                                                         piece.value(u"composer"_s).toString(), piece.value(u"title"_s).toString());
                    ok = ok && !m_store->create(u"activity"_s, {
                        { u"student_id"_s, copy.value(u"student_id"_s) }, { u"continuous"_s, 0 },
                        { u"description"_s, description }, { u"date"_s, e.value(u"date"_s) },
                        { u"single_kind"_s, copy.value(u"kind"_s).toInt() == 3 ? 1 : 0 } }).isEmpty();
                }
                if (finishPieces)
                    ok = ok && m_store->update(u"piece"_s, copy.value(u"id"_s).toString(),
                                               { { u"state"_s, 4 }, { u"end_date"_s, todayIso() } });
            }
        }
    }
    return m_store->endBatch(ok) && ok;
}

// ---------------------------------------------------------------------------- notices

QVariantList DataService::birthdays(int daysAhead) const
{
    const QDate today = Qupil::Clock::today();
    QList<QVariantMap> list;
    for (const QVariant &v : rows(u"SELECT id, forename, surname, birthday FROM student WHERE deleted = 0 AND birthday <> ''"_s)) {
        QVariantMap m = v.toMap();
        const QDate b = QDate::fromString(m.value(u"birthday"_s).toString(), Qt::ISODate);
        if (!b.isValid())
            continue;
        QDate next(today.year(), b.month(), b.day());
        if (!next.isValid())   // 29 February
            next = QDate(today.year(), 3, 1);
        if (next < today)
            next = next.addYears(1);
        const int days = int(today.daysTo(next));
        if (days > daysAhead)
            continue;
        m.insert(u"name"_s, fullName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        m.insert(u"initials"_s, initials(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        m.insert(u"days"_s, days);
        m.insert(u"date"_s, next.toString(Qt::ISODate));
        m.insert(u"turns"_s, next.year() - b.year());
        list << m;
    }
    std::sort(list.begin(), list.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return a.value(u"days"_s).toInt() < b.value(u"days"_s).toInt();
    });
    QVariantList out;
    for (const QVariantMap &m : list)
        out << m;
    return out;
}

QVariantList DataService::reminders() const
{
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT r.*, s.forename, s.surname FROM reminder r LEFT JOIN student s ON s.id = r.student_id "
                                  u"WHERE r.deleted = 0 ORDER BY r.mode, r.text COLLATE NOCASE"_s)) {
        QVariantMap m = v.toMap();
        m.insert(u"studentName"_s, fullName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        out << m;
    }
    return out;
}

QVariantList DataService::recitalCandidates() const
{
    const bool soloOnly = setting(u"recital_check_solo_only"_s, 1).toInt() != 0;
    const QDate today = Qupil::Clock::today();
    QList<QVariantMap> list;
    for (const QVariant &v : rows(u"SELECT id, forename, surname, recital_interval, first_lesson FROM student "
                                  u"WHERE deleted = 0 AND recital_interval > 0"_s)) {
        QVariantMap s = v.toMap();
        const QString id = s.value(u"id"_s).toString();
        const int interval = s.value(u"recital_interval"_s).toInt();
        if (interval <= 0 || interval >= int(std::size(kIntervalDays)))
            continue;
        if (scalar(u"SELECT COUNT(*) FROM membership WHERE deleted = 0 AND student_id = ? AND (end_date = '' OR end_date > ?)"_s,
                   { id, todayIso() }).toInt() == 0)
            continue;
        // Already in a programme
        if (scalar(u"SELECT COUNT(*) FROM event_item i JOIN piece p ON p.id = i.piece_id "
                   u"JOIN piece c ON c.group_key = p.group_key JOIN membership m ON m.id = c.membership_id "
                   u"JOIN event e ON e.id = i.event_id "
                   u"WHERE i.deleted = 0 AND e.deleted = 0 AND e.state = 0 AND m.student_id = ?"_s, { id }).toInt() > 0)
            continue;
        const QString last = scalar(soloOnly
                                        ? u"SELECT MAX(date) FROM activity WHERE deleted = 0 AND continuous = 0 AND single_kind = 0 AND student_id = ?"_s
                                        : u"SELECT MAX(date) FROM activity WHERE deleted = 0 AND continuous = 0 AND single_kind IN (0, 1) AND student_id = ?"_s,
                                    { id }).toString();
        QDate reference = QDate::fromString(last, Qt::ISODate);
        if (!reference.isValid())
            reference = QDate::fromString(s.value(u"first_lesson"_s).toString(), Qt::ISODate);
        if (!reference.isValid())
            continue;
        const int ago = int(reference.daysTo(today));
        if (ago <= kIntervalDays[interval])
            continue;
        s.insert(u"name"_s, fullName(s.value(u"forename"_s).toString(), s.value(u"surname"_s).toString()));
        s.insert(u"initials"_s, initials(s.value(u"forename"_s).toString(), s.value(u"surname"_s).toString()));
        s.insert(u"days"_s, ago);
        s.insert(u"lastDate"_s, reference.toString(Qt::ISODate));
        s.insert(u"hasRecital"_s, !last.isEmpty());
        list << s;
    }
    std::sort(list.begin(), list.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return a.value(u"days"_s).toInt() > b.value(u"days"_s).toInt();
    });
    QVariantList out;
    for (const QVariantMap &m : list)
        out << m;
    return out;
}

// ---------------------------------------------------------------------------- ensemble candidates

QVariantMap DataService::ensembleCandidates() const
{
    QList<QVariantMap> list;
    int active = 0;
    for (const QVariant &v : rows(u"SELECT id, forename, surname FROM student WHERE deleted = 0 AND ensemble_expected = 1"_s)) {
        QVariantMap s = v.toMap();
        const QString id = s.value(u"id"_s).toString();
        if (scalar(u"SELECT COUNT(*) FROM membership WHERE deleted = 0 AND student_id = ? AND (end_date = '' OR end_date > ?)"_s,
                   { id, todayIso() }).toInt() == 0)
            continue;
        // The latest ensemble (continuous activity of kind 0); open end = still playing
        const QVariantList last = rows(u"SELECT description, end_date FROM activity WHERE deleted = 0 AND continuous = 1 "
                                       u"AND continuous_kind = 0 AND student_id = ? "
                                       u"ORDER BY CASE WHEN end_date = '' THEN 1 ELSE 0 END DESC, end_date DESC LIMIT 1"_s, { id });
        const QVariantMap ensemble = last.isEmpty() ? QVariantMap() : last.first().toMap();
        const QString end = ensemble.value(u"end_date"_s).toString();
        if (!ensemble.isEmpty() && (end.isEmpty() || end > todayIso())) {   // open or still running
            ++active;
            continue;
        }
        s.insert(u"name"_s, fullName(s.value(u"forename"_s).toString(), s.value(u"surname"_s).toString()));
        s.insert(u"initials"_s, initials(s.value(u"forename"_s).toString(), s.value(u"surname"_s).toString()));
        s.insert(u"lastDate"_s, ensemble.value(u"end_date"_s).toString());
        s.insert(u"lastEnsemble"_s, ensemble.value(u"description"_s).toString());
        list << s;
    }
    std::sort(list.begin(), list.end(), [](const QVariantMap &a, const QVariantMap &b) {
        return a.value(u"lastDate"_s).toString() > b.value(u"lastDate"_s).toString();
    });
    QVariantList out;
    for (const QVariantMap &m : list)
        out << m;
    return { { u"candidates"_s, out }, { u"active"_s, active } };
}

// ---------------------------------------------------------------------------- library

int DataService::nextLibraryNumber() const
{
    return scalar(u"SELECT COALESCE(MAX(number), 0) FROM library_item WHERE deleted = 0"_s).toInt() + 1;
}

QVariantList DataService::libraryItems(int mode, const QString &filter) const
{
    QString where = u"l.deleted = 0"_s;
    if (mode == 1)
        where += u" AND (l.lent_to = '' OR s.id IS NULL)"_s;
    else if (mode == 2)
        where += u" AND l.lent_to <> '' AND s.id IS NOT NULL"_s;
    QVariantList out;
    const QString needle = filter.trimmed();
    QCollator collator(uiLocale());
    collator.setCaseSensitivity(Qt::CaseInsensitive);
    QList<QVariantMap> list;
    for (const QVariant &v : rows(u"SELECT l.*, s.forename, s.surname FROM library_item l "
                                  u"LEFT JOIN student s ON s.id = l.lent_to AND s.deleted = 0 WHERE "_s + where)) {
        QVariantMap m = v.toMap();
        const bool lent = !m.value(u"surname"_s).isNull() || !m.value(u"forename"_s).isNull();
        const QString borrower = lent ? fullName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()) : QString();
        m.insert(u"lent"_s, lent);
        m.insert(u"borrower"_s, borrower);
        if (!needle.isEmpty()) {
            const int number = m.value(u"number"_s).toInt();
            const QString haystack = m.value(u"author"_s).toString() + u' ' + m.value(u"title"_s).toString() + u' '
                                     + m.value(u"publisher"_s).toString() + u' ' + borrower
                                     + (number > 0 ? u' ' + QString::number(number).rightJustified(4, u'0') : QString());
            if (!haystack.contains(needle, Qt::CaseInsensitive))
                continue;
        }
        list << m;
    }
    std::sort(list.begin(), list.end(), [&collator](const QVariantMap &a, const QVariantMap &b) {
        const int c = collator.compare(a.value(u"author"_s).toString(), b.value(u"author"_s).toString());
        return c != 0 ? c < 0 : collator.compare(a.value(u"title"_s).toString(), b.value(u"title"_s).toString()) < 0;
    });
    for (const QVariantMap &m : list)
        out << m;
    return out;
}

int DataService::lentCount() const
{
    return scalar(u"SELECT COUNT(*) FROM library_item l JOIN student s ON s.id = l.lent_to AND s.deleted = 0 "
                  u"WHERE l.deleted = 0"_s).toInt();
}

bool DataService::lendLibraryItem(const QString &itemId, const QString &studentId)
{
    return m_store->update(u"library_item"_s, itemId, { { u"lent_to"_s, studentId }, { u"lent_since"_s, todayIso() } });
}

bool DataService::returnLibraryItem(const QString &itemId)
{
    // lent_since stays: when it was lent the last time
    return m_store->update(u"library_item"_s, itemId, { { u"lent_to"_s, QString() } });
}

QStringList DataService::libraryAuthors() const
{
    QStringList out;
    for (const QVariant &v : rows(u"SELECT DISTINCT author FROM library_item WHERE deleted = 0 AND author <> '' "
                                  u"ORDER BY author COLLATE NOCASE"_s))
        out << v.toMap().value(u"author"_s).toString();
    return out;
}

QStringList DataService::libraryPublishers() const
{
    QStringList out;
    for (const QVariant &v : rows(u"SELECT DISTINCT publisher FROM library_item WHERE deleted = 0 AND publisher <> '' "
                                  u"ORDER BY publisher COLLATE NOCASE"_s))
        out << v.toMap().value(u"publisher"_s).toString();
    return out;
}

// ---------------------------------------------------------------------------- instruments

QVariantMap DataService::instrumentOverview() const
{
    QVariantList students;
    QMap<QString, int> byInstrument;
    QMap<QString, int> bySize;
    int rental = 0;
    int nextSize = 0;
    for (const QVariant &v : rows(u"SELECT id, forename, surname, birthday, instrument, instrument_size, rental, "
                                  u"rental_desc, rental_since, next_size FROM student WHERE deleted = 0 "
                                  u"AND (instrument <> '' OR instrument_size <> '' OR rental = 1) "
                                  u"ORDER BY surname COLLATE NOCASE, forename COLLATE NOCASE"_s)) {
        QVariantMap m = v.toMap();
        m.insert(u"name"_s, listName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        m.insert(u"initials"_s, initials(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        const QString instrument = m.value(u"instrument"_s).toString();
        const QString size = m.value(u"instrument_size"_s).toString();
        if (!instrument.isEmpty())
            ++byInstrument[instrument];
        if (!size.isEmpty())
            ++bySize[size];
        rental += m.value(u"rental"_s).toInt() == 1;
        nextSize += m.value(u"next_size"_s).toInt() == 1;
        students << m;
    }
    auto counts = [](const QMap<QString, int> &map) {
        QVariantList out;
        for (auto it = map.cbegin(); it != map.cend(); ++it)
            out << QVariantMap { { u"name"_s, it.key() }, { u"count"_s, it.value() } };
        return out;
    };
    return { { u"students"_s, students }, { u"byInstrument"_s, counts(byInstrument) },
             { u"bySize"_s, counts(bySize) }, { u"rentalCount"_s, rental }, { u"nextSizeCount"_s, nextSize } };
}

// ---------------------------------------------------------------------------- archives

QVariantList DataService::studentArchive() const
{
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT id, forename, surname, archived_on FROM student_archive WHERE deleted = 0 "
                                  u"ORDER BY surname COLLATE NOCASE, forename COLLATE NOCASE"_s)) {
        QVariantMap m = v.toMap();
        m.insert(u"name"_s, listName(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        m.insert(u"initials"_s, initials(m.value(u"forename"_s).toString(), m.value(u"surname"_s).toString()));
        out << m;
    }
    return out;
}

QVariantList DataService::eventArchive() const
{
    // Titles start with "yyyy-MM-dd_HH:mm" (as in Qupil 1.x): newest first
    return rows(u"SELECT id, title FROM event_archive WHERE deleted = 0 ORDER BY title DESC"_s);
}

// ---------------------------------------------------------------------------- CSV import

namespace {

const char *const kCsvFields[] = { "mobile", "phone", "email", "forename", "surname", "birthday", "zip", "city", "street" };

//! Words that column titles of common address books contain (several languages)
QStringList csvPatterns(const QString &field)
{
    if (field == u"forename"_s)
        return { u"first name"_s, u"firstname"_s, u"given name"_s, u"vorname"_s, u"prénom"_s, u"prenom"_s, u"nombre"_s };
    if (field == u"surname"_s)
        return { u"last name"_s, u"lastname"_s, u"surname"_s, u"family name"_s, u"nachname"_s, u"familienname"_s,
                 u"nom de famille"_s, u"apellido"_s, u"nom"_s };
    if (field == u"birthday"_s)
        return { u"birthday"_s, u"birth"_s, u"geburtstag"_s, u"geburtsdatum"_s, u"anniversaire"_s, u"naissance"_s,
                 u"cumpleaños"_s, u"nacimiento"_s };
    if (field == u"street"_s)
        return { u"street"_s, u"straße"_s, u"strasse"_s, u"adresse"_s, u"address"_s, u"rue"_s, u"calle"_s, u"dirección"_s };
    if (field == u"zip"_s)
        return { u"zip"_s, u"postal"_s, u"postcode"_s, u"plz"_s, u"postleitzahl"_s, u"code postal"_s, u"código postal"_s };
    if (field == u"city"_s)
        return { u"city"_s, u"town"_s, u"ort"_s, u"stadt"_s, u"ville"_s, u"ciudad"_s };
    if (field == u"email"_s)
        return { u"mail"_s };
    if (field == u"mobile"_s)
        return { u"mobile"_s, u"cell"_s, u"handy"_s, u"mobil"_s, u"portable"_s, u"móvil"_s };
    if (field == u"phone"_s)
        return { u"phone"_s, u"telefon"_s, u"téléphone"_s, u"teléfono"_s, u"tel"_s };
    return {};
}

QByteArray readFile(const QUrl &url)
{
    QFile f(url.isLocalFile() ? url.toLocalFile() : url.toString());   // content:// on Android
    if (!f.open(QIODevice::ReadOnly))
        return {};
    return f.readAll();
}

} // namespace

QVariantMap DataService::readCsv(const QUrl &file, bool utf8) const
{
    const QByteArray data = readFile(file);
    if (data.isEmpty())
        return { { u"error"_s, u"open"_s } };
    const auto table = Qupil::Csv::parse(data, utf8);
    if (!table)
        return { { u"error"_s, u"separator"_s } };
    QVariantMap suggested;
    QSet<int> taken;
    for (const char *f : kCsvFields) {
        const QString field = QString::fromLatin1(f);
        int column = -1;
        for (const QString &pattern : csvPatterns(field)) {
            for (int i = 0; i < table->headers.size() && column < 0; ++i) {
                if (taken.contains(i))
                    continue;
                const QString header = table->headers.at(i).toLower();
                // "nom" and "tel" only as whole words (not "Nombre", "Hotel")
                const bool word = pattern.size() <= 3;
                const bool match = word ? QRegularExpression(u"\\b"_s + QRegularExpression::escape(pattern) + u"\\b"_s,
                                                             QRegularExpression::UseUnicodePropertiesOption)
                                              .match(header).hasMatch()
                                        : header.contains(pattern);
                if (match)
                    column = i;
            }
            if (column >= 0)
                break;
        }
        if (column >= 0)
            taken.insert(column);
        suggested.insert(field, column);
    }
    return { { u"headers"_s, table->headers }, { u"count"_s, table->rows.size() }, { u"suggested"_s, suggested },
             { u"error"_s, QString() } };
}

int DataService::importCsv(const QUrl &file, bool utf8, const QVariantMap &mapping)
{
    const auto table = Qupil::Csv::parse(readFile(file), utf8);
    if (!table)
        return 0;
    auto value = [&mapping](const QStringList &row, const char *field) {
        const int column = mapping.value(QString::fromLatin1(field), -1).toInt();
        return column >= 0 && column < row.size() ? row.at(column).trimmed() : QString();
    };
    int count = 0;
    m_store->beginBatch();
    for (const QStringList &row : table->rows) {
        const QString forename = value(row, "forename");
        const QString surname = value(row, "surname");
        if (forename.isEmpty() && surname.isEmpty())
            continue;   // no name, no student
        const QString street = value(row, "street");
        const QString place = (value(row, "zip") + u' ' + value(row, "city")).trimmed();
        QStringList address;
        if (!street.isEmpty())
            address << street;
        if (!place.isEmpty())
            address << place;
        m_store->create(u"student"_s, { { u"forename"_s, forename }, { u"surname"_s, surname },
                                        { u"birthday"_s, Qupil::Csv::isoDate(value(row, "birthday")) },
                                        { u"address"_s, address.join(u", "_s) }, { u"email"_s, value(row, "email") },
                                        { u"phone"_s, value(row, "phone") }, { u"mobile"_s, value(row, "mobile") },
                                        { u"first_lesson"_s, todayIso() } });
        ++count;
    }
    m_store->endBatch();
    return count;
}

