// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "DocumentService.h"

#include <QCoreApplication>
#include <QDate>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFont>
#include <QJSEngine>
#include <QLocale>
#include <QPageLayout>
#include <QPageSize>
#include <QPdfWriter>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QAbstractTextDocumentLayout>
#include <QTextDocument>
#include <QTextFrame>
#include <QTimer>
#include <QThread>

#include "ArchiveFormat.h"
#include "DataService.h"
#include "Database.h"
#include "FileExport.h"
#include "LanguageManager.h"
#include "Store.h"
#include "TextPack.h"

#include <algorithm>
#include <memory>

using namespace Qt::StringLiterals;

namespace {

DocumentService *s_instance = nullptr;
constexpr int kKeepDocuments = 30;

QString esc(const QString &s)
{
    return s.toHtmlEscaped();
}

QString escLines(const QString &s)
{
    return esc(s).replace(u'\n', u"<br>"_s);
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

//! A label from the catalog without the colon that Qupil 1.x put at the end
QString label(const char *id)
{
    return qtTrId(id).remove(QRegularExpression(u"\\s*:\\s*$"_s));
}

QString page(const QString &title, const QString &body, bool footer = true)
{
    // Simple layout that QTextDocument renders the same everywhere (.m: marked note text)
    return u"<html><head><meta charset=\"utf-8\"><title>"_s + esc(title) + u"</title><style>"
           u"h1 { font-size: 17pt; margin-bottom: 2pt; } h2 { font-size: 13pt; margin-top: 14pt; } "
           u"h3 { font-size: 11pt; margin-top: 10pt; } th { background-color: #ECEAF0; text-align: left; } "
           u"td, th { padding: 3pt; vertical-align: top; } .muted { color: #5E5866; } .m { color: #B3261E; } "
           u"</style></head><body>"_s
           + body
           + (footer ? u"<p class=\"muted\"><i>Qupil "_s + QCoreApplication::applicationVersion().section(u'.', 0, 1)
                           + u" – © 2006–"_s + QString::number(QDate::currentDate().year())
                           + u" Felix Hammer – qupil.de</i></p>"_s
                     : QString())
           + u"</body></html>"_s;
}

QString table(const QStringList &headers, const QList<QStringList> &rows, bool escapeCells = true)
{
    QString html = u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">"_s;
    if (!headers.isEmpty()) {
        html += u"<tr>"_s;
        for (const QString &h : headers)
            html += u"<th>"_s + esc(h) + u"</th>"_s;
        html += u"</tr>"_s;
    }
    for (const QStringList &row : rows) {
        html += u"<tr>"_s;
        for (const QString &cell : row)
            html += u"<td>"_s + (escapeCells ? escLines(cell) : cell) + u"</td>"_s;
        html += u"</tr>"_s;
    }
    return html + u"</table>"_s;
}

QString pieceState(int state)
{
    switch (state) {
    case 1: return qtTrId("piece.status.in_progress");
    case 2: return qtTrId("piece.status.paused");
    case 3: return qtTrId("piece.status.ready");
    case 4: return qtTrId("piece.status.finished");
    default: return qtTrId("piece.status.planned");
    }
}

QString fileSafe(const QString &name)
{
    QString s = name;
    s.replace(QRegularExpression(u"[\\\\/:*?\"<>|\\s]+"_s), u"-"_s);
    return s.left(80);
}

} // namespace

DocumentService::DocumentService(DataService *data, QObject *parent)
    : QObject(parent)
    , m_data(data)
{
    Q_ASSERT_X(!s_instance, "DocumentService", "only one instance allowed");
    s_instance = this;
}

DocumentService::~DocumentService()
{
    m_legacyArchives.clear();
    setArchiveUpkeep(false);
    if (m_converter) {
        m_converter->wait();   // one entry: a second or two
        delete m_converter;
    }
    if (s_instance == this)
        s_instance = nullptr;
}

DocumentService *DocumentService::instance()
{
    return s_instance;
}

DocumentService *DocumentService::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "DocumentService", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- PDF

QUrl DocumentService::writePdf(const QString &html, const QString &baseName, bool landscape, const QString &target) const
{
    // In the app's files: on Android only these can be handed to the viewer
    const QString folder = !target.isEmpty() ? target
                                             : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + u"/documents"_s;
    if (!QDir().mkpath(folder))
        return {};
    // Keep the folder small: the newest documents only
    if (target.isEmpty()) {
        QFileInfoList old = QDir(folder).entryInfoList({ u"*.pdf"_s }, QDir::Files, QDir::Time);
        for (int i = kKeepDocuments; i < old.size(); ++i)
            QFile::remove(old.at(i).absoluteFilePath());
    }

    const QString path = folder + u'/' + fileSafe(baseName) + u".pdf"_s;
    QPdfWriter writer(path);
    writer.setTitle(baseName);
    writer.setCreator(u"Qupil"_s);
    writer.setResolution(300);
    writer.setPageLayout(QPageLayout(QPageSize(QPageSize::A4),
                                     landscape ? QPageLayout::Landscape : QPageLayout::Portrait,
                                     QMarginsF(10, 12, 10, 12), QPageLayout::Millimeter));
    // Laid out for the PDF's resolution and page (print() would add 2 cm margins of its own)
    QTextDocument doc;
    doc.documentLayout()->setPaintDevice(&writer);
    doc.setDefaultFont(QFont(u"Figtree"_s, 10));
    doc.setHtml(html);
    QTextFrameFormat frame = doc.rootFrame()->frameFormat();
    frame.setMargin(0);
    doc.rootFrame()->setFrameFormat(frame);
    doc.setPageSize(writer.pageLayout().paintRectPixels(writer.resolution()).size());
    doc.print(&writer);
    if (!QFileInfo(path).size())
        return {};
    return QUrl::fromLocalFile(path);
}

QUrl DocumentService::writeAndOpen(const QString &html, const QString &baseName, bool landscape)
{
    if (FileExport::available()) {
        // iOS: the user chooses where the PDF goes ("Save to Files"); Qt cannot open it here
        const QString folder = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)).filePath(u"pdf"_s);
        QDir(folder).removeRecursively();
        const QUrl url = writePdf(html, baseName, landscape, folder);
        if (url.isEmpty()) {
            emit failed(qtTrId("doc.error.write"));
            return {};
        }
        const QString path = url.toLocalFile();
        FileExport::exportFile(path, [this, path](bool saved, const QString &name) {
            QFile::remove(path);
            //: %1 = file name
            if (saved)
                emit exported(qtTrId("doc.saved").arg(name.isEmpty() ? QFileInfo(path).fileName() : name));
        });
        return url;
    }
    const QUrl url = writePdf(html, baseName, landscape);
    if (url.isEmpty()) {
        emit failed(qtTrId("doc.error.write"));
        return {};
    }
    if (!QDesktopServices::openUrl(url))
        emit failed(qtTrId("doc.error.open").arg(QDir::toNativeSeparators(url.toLocalFile())));
    return url;
}

QUrl DocumentService::dayOverview(const QString &dateIso, int notesPerLesson, int spaceLines)
{
    return writeAndOpen(dayOverviewHtml(dateIso, notesPerLesson, spaceLines),
                        qtTrId("doc.dayview.title") + u' ' + (dateIso.isEmpty() ? qtTrId("common.irregular") : dateIso),
                        false);
}

QUrl DocumentService::timetable()
{
    return writeAndOpen(timetableHtml(), qtTrId("doc.timetable.title") + u' ' + QDate::currentDate().toString(Qt::ISODate),
                        false);
}

QUrl DocumentService::rentalList()
{
    return writeAndOpen(rentalListHtml(),
                        qtTrId("instruments.inventory_list") + u' ' + QDate::currentDate().toString(Qt::ISODate), false);
}

QUrl DocumentService::programme(const QString &eventId)
{
    const QVariantMap e = m_data->get(u"event"_s, eventId);
    // As in Qupil 1.x: "<title>_<location>_<date>"
    const QString name = QStringList { e.value(u"title"_s).toString(), e.value(u"location"_s).toString(),
                                       e.value(u"date"_s).toString() }.join(u'_');
    return writeAndOpen(programmeHtml(eventId), name, true);
}

QUrl DocumentService::archiveEntry(const QString &entity, const QString &id)
{
    if (entity != u"student_archive"_s && entity != u"event_archive"_s)
        return {};
    const QVariantMap entry = m_data->get(entity, id);
    if (entry.isEmpty())
        return {};
    const QString name = entity == u"student_archive"_s
                             ? qtTrId("archive.entry") + u' '
                                   + listName(entry.value(u"forename"_s).toString(), entry.value(u"surname"_s).toString())
                             : entry.value(u"title"_s).toString();
    return writeAndOpen(archiveHtml(entity, id), name, entity == u"event_archive"_s);
}

QString DocumentService::compactLegacyArchive(const QString &legacyHtml)
{
    QString title;
    const QString body = ArchiveFormat::compactBody(legacyHtml, &title);
    return page(title, body, false);   // the old entry has its own footer
}

QString DocumentService::archiveHtml(const QString &entity, const QString &id)
{
    if (entity != u"student_archive"_s && entity != u"event_archive"_s)
        return {};
    const QString stored = m_data->get(entity, id).value(u"content"_s).toString();
    const QString html = TextPack::unpack(stored);
    if (!ArchiveFormat::isLegacy(html))
        return html;
    // Opening it takes as long as converting it: convert, and keep that (after this call,
    // so a view that shows it is not changed while it asks)
    const QString compact = compactLegacyArchive(html);
    QTimer::singleShot(0, this, [this, entity, id, stored, compact] {
        m_data->m_store->rewrite(entity, id, u"content"_s, stored, TextPack::pack(compact));
    });
    return compact;
}

void DocumentService::setArchiveUpkeep(bool on)
{
    if (!on) {
        delete m_archiveScan;
        m_archiveScan = nullptr;
        return;
    }
    if (m_archiveScan)
        return;
    m_archiveScan = new QTimer(this);
    m_archiveScan->setSingleShot(true);
    m_archiveScan->setInterval(20000);   // once things are calm
    connect(m_archiveScan, &QTimer::timeout, this, &DocumentService::convertLegacyArchives);
    connect(m_data->m_store, &Qupil::Store::dataChanged, this, [this](const QStringList &entities) {
        if (m_archiveScan && !m_converter
            && (entities.contains(u"student_archive"_s) || entities.contains(u"event_archive"_s)))
            m_archiveScan->start();
    });
    m_archiveScan->start();
}

void DocumentService::convertLegacyArchives()
{
    if (m_converter)
        return;   // running; it goes on with the list
    m_legacyArchives.clear();
    for (const QString &entity : { u"student_archive"_s, u"event_archive"_s }) {
        for (const QVariant &v : m_data->rows(u"SELECT id, content FROM %1 WHERE deleted = 0"_s.arg(entity), {})) {
            const QVariantMap row = v.toMap();
            const QString stored = row.value(u"content"_s).toString();
            const QString id = row.value(u"id"_s).toString();
            if (ArchiveFormat::isLegacy(TextPack::unpack(stored)))
                m_legacyArchives.append({ entity, id });
            else if (!stored.isEmpty() && !TextPack::isPacked(stored))
                m_data->m_store->rewrite(entity, id, u"content"_s, stored, TextPack::pack(stored));
        }
    }
    convertNext();
}

void DocumentService::convertNext()
{
    if (m_converter)
        return;   // one at a time; it goes on with the list
    while (!m_legacyArchives.isEmpty()) {
        const QPair<QString, QString> next = m_legacyArchives.takeFirst();
        const QString entity = next.first;
        const QString id = next.second;
        const QString stored = m_data->get(entity, id).value(u"content"_s).toString();
        const QString legacy = TextPack::unpack(stored);
        if (!ArchiveFormat::isLegacy(legacy))
            continue;   // converted meanwhile (opened, or by another device)
        auto result = std::make_shared<QString>();
        m_converter = QThread::create([legacy, result] { *result = TextPack::pack(compactLegacyArchive(legacy)); });
        connect(m_converter, &QThread::finished, this, [this, entity, id, stored, result] {
            m_converter->deleteLater();
            m_converter = nullptr;
            // In place, also in the change log: every device converts its own entries, so the
            // old layout does not stay in the log and nothing needs to be sent (only where
            // the entry is still the same)
            if (!result->isEmpty() && m_data->m_store->rewrite(entity, id, u"content"_s, stored, *result))
                ++m_converted;
            QTimer::singleShot(200, this, &DocumentService::convertNext);
        });
        m_converter->start(QThread::LowPriority);
        return;
    }
    if (m_converted > 0) {
        // All done: give the space of the old layout back
        m_converted = 0;
        QSqlQuery(m_data->m_store->database()->connection()).exec(u"VACUUM"_s);
    }
}

QVariantList DocumentService::dayChoices() const
{
    const QLocale locale = uiLocale();
    const QDate today = QDate::currentDate();
    QVariantList out;
    for (int weekday = 0; weekday < 7; ++weekday) {
        // Today, or the next day with this weekday
        const int ahead = (weekday + 1 - today.dayOfWeek() + 7) % 7;
        const QDate date = today.addDays(ahead);
        out << QVariantMap { { u"weekday"_s, weekday }, { u"date"_s, date.toString(Qt::ISODate) },
                             { u"label"_s, locale.dayName(weekday + 1) + u" – "_s + m_data->formatDate(date.toString(Qt::ISODate)) } };
    }
    out << QVariantMap { { u"weekday"_s, 7 }, { u"date"_s, QString() }, { u"label"_s, qtTrId("common.irregular") } };
    return out;
}

// ---------------------------------------------------------------------------- daily overview

QString DocumentService::dayOverviewHtml(const QString &dateIso, int notesPerLesson, int spaceLines) const
{
    const QDate date = QDate::fromString(dateIso, Qt::ISODate);
    const bool irregular = !date.isValid();
    const QVariantList lessons = irregular ? m_data->irregularLessons() : m_data->lessonsOn(date.dayOfWeek() - 1);
    const bool shared = m_data->setting(u"save_for_all_students"_s, 1).toInt() != 0;
    const QString heading = qtTrId("doc.dayview.title") + u" – "_s
                            + (irregular ? qtTrId("schedule.irregular_dates") : uiLocale().toString(date, QLocale::LongFormat));

    QString body = u"<h1>"_s + esc(heading) + u"</h1>"_s;
    body += u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\"><tr>"_s;
    if (!irregular)
        body += u"<th width=\"15%\">"_s + esc(qtTrId("common.time")) + u"</th>"_s;
    body += u"<th>"_s + esc(qtTrId("common.lesson")) + u"</th><th>"_s + esc(qtTrId("common.student")) + u"</th></tr>"_s;
    const int columns = irregular ? 2 : 3;

    for (const QVariant &v : lessons) {
        const QVariantMap lesson = v.toMap();
        const QVariantList students = lesson.value(u"students"_s).toList();
        QStringList names;
        for (const QVariant &s : students)
            names << esc(s.toMap().value(u"listName"_s).toString());
        body += u"<tr>"_s;
        if (!irregular)
            body += u"<td><b>"_s + esc(lesson.value(u"start_time"_s).toString() + u" – "_s + lesson.value(u"end_time"_s).toString())
                    + u"</b></td>"_s;
        body += u"<td><b>"_s + esc(lesson.value(u"name"_s).toString()) + u"</b></td><td>"_s + names.join(u"<br>"_s)
                + u"</td></tr>"_s;

        // Room for handwriting, then the latest notes
        QString notes;
        for (int i = 0; i < spaceLines; ++i)
            notes += u"<p>&nbsp;</p>"_s;
        if (spaceLines > 0)
            notes += u"<hr>"_s;
        if (notesPerLesson > 0) {
            // Shared notes are the same for everyone: show them once
            const QVariantList whose = shared && !students.isEmpty() ? QVariantList { students.last() } : students;
            QString list;
            for (const QVariant &s : whose) {
                const QVariantMap student = s.toMap();
                const QVariantList latest = m_data->rows(
                    u"SELECT date, content FROM note WHERE deleted = 0 AND membership_id = ? ORDER BY date DESC LIMIT ?"_s,
                    { student.value(u"membership_id"_s), notesPerLesson });
                for (const QVariant &n : latest) {
                    const QVariantMap note = n.toMap();
                    QString when = u"<u>"_s + esc(m_data->formatDate(note.value(u"date"_s).toString())) + u"</u>"_s;
                    if (!shared || whose.size() > 1)
                        when += u"<br><i>"_s + esc(student.value(u"listName"_s).toString()) + u"</i>"_s;
                    list += u"<tr><td width=\"18%\">"_s + when + u"</td><td>"_s
                            + m_data->noteHtml(note.value(u"content"_s).toString()) + u"</td></tr>"_s;
                }
            }
            if (!list.isEmpty())
                notes += u"<table border=\"0\" cellspacing=\"0\" cellpadding=\"2\" width=\"100%\">"_s + list + u"</table>"_s;
        }
        if (!notes.isEmpty())
            body += u"<tr><td colspan=\"%1\">"_s.arg(columns) + notes + u"</td></tr>"_s;
    }
    body += u"</table>"_s;
    if (lessons.isEmpty())
        body += u"<p>"_s + esc(qtTrId("schedule.no_lessons_day")) + u"</p>"_s;
    return page(heading, body);
}

// ---------------------------------------------------------------------------- timetable

QString DocumentService::timetableHtml() const
{
    const QLocale locale = uiLocale();
    const QString title = qtTrId("doc.timetable.title");
    QString body = u"<h1>"_s + esc(title) + u"</h1><p>"_s + esc(qtTrId("doc.as_of").arg(m_data->formatDate(m_data->today())))
                   + u"</p>"_s;
    body += u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\"><tr><th width=\"15%\">"_s
            + esc(qtTrId("common.time")) + u"</th><th>"_s + esc(qtTrId("common.lesson")) + u"</th><th>"_s
            + esc(qtTrId("nav.students")) + u"</th></tr>"_s;
    auto names = [](const QVariantMap &lesson) {
        QStringList out;
        for (const QVariant &s : lesson.value(u"students"_s).toList())
            out << esc(s.toMap().value(u"listName"_s).toString());
        return out.join(u"<br>"_s);
    };
    for (int weekday = 0; weekday < 7; ++weekday) {
        const QVariantList lessons = m_data->lessonsOn(weekday);
        if (lessons.isEmpty())
            continue;
        body += u"<tr><td colspan=\"3\"><h2>"_s + esc(locale.dayName(weekday + 1)) + u"</h2></td></tr>"_s;
        for (const QVariant &v : lessons) {
            const QVariantMap l = v.toMap();
            body += u"<tr><td><b>"_s + esc(l.value(u"start_time"_s).toString() + u" – "_s + l.value(u"end_time"_s).toString())
                    + u"</b></td><td>"_s + esc(l.value(u"name"_s).toString()) + u"</td><td>"_s + names(l) + u"</td></tr>"_s;
        }
    }
    const QVariantList irregular = m_data->irregularLessons();
    if (!irregular.isEmpty()) {
        body += u"<tr><td colspan=\"3\"><h2>"_s + esc(qtTrId("common.irregular")) + u"</h2></td></tr>"_s;
        for (const QVariant &v : irregular) {
            const QVariantMap l = v.toMap();
            body += u"<tr><td colspan=\"2\"><b>"_s + esc(l.value(u"name"_s).toString()) + u"</b></td><td>"_s + names(l)
                    + u"</td></tr>"_s;
        }
    }
    body += u"</table>"_s;
    return page(title, body);
}

// ---------------------------------------------------------------------------- rental instruments

QString DocumentService::rentalListHtml() const
{
    const QString title = qtTrId("instruments.inventory_list");
    QList<QStringList> rows;
    for (const QVariant &v : m_data->rows(u"SELECT * FROM student WHERE deleted = 0 AND rental = 1 "
                                          u"ORDER BY surname COLLATE NOCASE, forename COLLATE NOCASE"_s)) {
        const QVariantMap s = v.toMap();
        rows << QStringList { listName(s.value(u"forename"_s).toString(), s.value(u"surname"_s).toString()),
                              m_data->formatDate(s.value(u"birthday"_s).toString()),
                              s.value(u"instrument"_s).toString(), s.value(u"instrument_size"_s).toString(),
                              s.value(u"rental_desc"_s).toString(),
                              m_data->formatDate(s.value(u"rental_since"_s).toString()),
                              s.value(u"next_size"_s).toInt() == 1 ? qtTrId("common.yes") : qtTrId("common.no") };
    }
    QString body = u"<h1>"_s + esc(title) + u"</h1><p>"_s + esc(qtTrId("doc.as_of").arg(m_data->formatDate(m_data->today())))
                   + u"</p>"_s;
    body += table({ qtTrId("common.name"), qtTrId("common.birthday"), qtTrId("common.instrument"), qtTrId("common.size"),
                    qtTrId("common.description"), qtTrId("student.rented_since"), qtTrId("instruments.next_size") },
                  rows);
    return page(title, body);
}

// ---------------------------------------------------------------------------- event programme

QString DocumentService::programmeHtml(const QString &eventId) const
{
    const QVariantMap e = m_data->event(eventId);
    const QString title = e.value(u"title"_s).toString();
    QString body = u"<h1>"_s + esc(qtTrId("event.program.heading").arg(title)) + u"</h1>"_s;
    QStringList info;
    auto add = [&info, &e](const char *labelId, const QString &value) {
        if (!value.isEmpty())
            info << u"<b>"_s + esc(label(labelId)) + u":</b> "_s + esc(value);
    };
    add("common.label.date", m_data->formatDate(e.value(u"date"_s).toString(), true));
    add("common.label.time", e.value(u"time"_s).toString());
    add("event.label.location", e.value(u"location"_s).toString());
    add("event.label.organizer", e.value(u"organizer"_s).toString());
    add("event.label.accompanist", e.value(u"accompanist"_s).toString());
    body += u"<p>"_s + info.join(u" &nbsp;·&nbsp; "_s) + u"</p>"_s;

    QList<QStringList> rows;
    for (const QVariant &v : e.value(u"programme"_s).toList()) {
        const QVariantMap item = v.toMap();
        const int minutes = item.value(u"duration"_s).toInt();
        rows << QStringList { QString::number(item.value(u"position"_s).toInt()), item.value(u"composer"_s).toString(),
                              item.value(u"title"_s).toString(), item.value(u"genre"_s).toString(),
                              minutes > 0 ? u"%1 %2"_s.arg(minutes).arg(qtTrId("common.unit.min")) : QString(),
                              item.value(u"performers"_s).toString().replace(u" · "_s, u"\n"_s) };
    }
    body += table({ u"#"_s, qtTrId("common.composer"), qtTrId("event.program.col_piece"), qtTrId("common.genre"),
                    qtTrId("common.duration"), qtTrId("event.col.student_age_instrument") },
                  rows);
    body += u"<p><b>"_s + esc(qtTrId("event.net_playing_time")) + u":</b> "_s
            + esc(u"%1 %2"_s.arg(e.value(u"playingMinutes"_s).toInt()).arg(qtTrId("common.unit.min")))
            + u" &nbsp;·&nbsp; <b>"_s + esc(qtTrId("event.estimated_total")) + u":</b> "_s
            + esc(u"%1 %2"_s.arg(e.value(u"totalMinutes"_s).toInt()).arg(qtTrId("common.unit.min"))) + u"</p>"_s;
    return page(qtTrId("event.program.doc_title"), body);
}

QString DocumentService::archiveEvent(const QString &eventId)
{
    const QVariantMap e = m_data->get(u"event"_s, eventId);
    if (e.isEmpty())
        return {};
    // "2026-11-02_16:00 Herbstvorspiel" (as in Qupil 1.x): sorts by date, same title replaces
    const QString title = (e.value(u"date"_s).toString() + u'_' + e.value(u"time"_s).toString() + u' '
                           + e.value(u"title"_s).toString()).trimmed();
    const QString html = programmeHtml(eventId);
    for (const QVariant &v : m_data->eventArchive()) {
        const QVariantMap a = v.toMap();
        if (a.value(u"title"_s).toString() == title) {
            m_data->update(u"event_archive"_s, a.value(u"id"_s).toString(), { { u"content"_s, TextPack::pack(html) } });
            return a.value(u"id"_s).toString();
        }
    }
    // Stored packed: archive entries are large and rarely opened
    return m_data->create(u"event_archive"_s, { { u"title"_s, title }, { u"content"_s, TextPack::pack(html) } });
}

// ---------------------------------------------------------------------------- student archive

QString DocumentService::studentArchiveHtml(const QString &studentId) const
{
    const QVariantMap s = m_data->get(u"student"_s, studentId);
    const QString forename = s.value(u"forename"_s).toString();
    const QString surname = s.value(u"surname"_s).toString();
    const QString name = (forename + u' ' + surname).trimmed();
    QString body = u"<h1>"_s + esc(qtTrId("archive.entry") + u": "_s + name) + u"</h1><p>"_s
                   + esc(label("common.label.date") + u": "_s + m_data->formatDate(m_data->today())) + u"</p>"_s;

    // Personal data
    QList<QStringList> personal;
    auto field = [&personal, &s](const QString &title, const QString &value) {
        if (!value.trimmed().isEmpty())
            personal << QStringList { title, value };
    };
    field(qtTrId("common.first_name"), forename);
    field(qtTrId("common.last_name"), surname);
    field(qtTrId("common.address"), s.value(u"address"_s).toString());
    field(qtTrId("common.email"), s.value(u"email"_s).toString());
    field(qtTrId("common.phone"), s.value(u"phone"_s).toString());
    field(qtTrId("common.mobile"), s.value(u"mobile"_s).toString());
    field(qtTrId("common.birthday"), m_data->formatDate(s.value(u"birthday"_s).toString()));
    field(qtTrId("student.father_name"), s.value(u"father_name"_s).toString());
    field(qtTrId("student.father_job"), s.value(u"father_job"_s).toString());
    field(qtTrId("student.father_phone"), s.value(u"father_phone"_s).toString());
    field(qtTrId("student.mother_name"), s.value(u"mother_name"_s).toString());
    field(qtTrId("student.mother_job"), s.value(u"mother_job"_s).toString());
    field(qtTrId("student.mother_phone"), s.value(u"mother_phone"_s).toString());
    const QDate first = QDate::fromString(s.value(u"first_lesson"_s).toString(), Qt::ISODate);
    if (first.isValid()) {
        const int years = std::max(0, m_data->ageOn(first.toString(Qt::ISODate)));
        field(qtTrId("student.lesson_since"),
              m_data->formatDate(first.toString(Qt::ISODate)) + u" ("_s + qtTrId("doc.total_years", years) + u')');
    }
    field(qtTrId("common.instrument"), s.value(u"instrument"_s).toString());
    field(qtTrId("common.size"), s.value(u"instrument_size"_s).toString());
    if (s.value(u"rental"_s).toInt() == 1) {
        field(qtTrId("student.rental_description"), s.value(u"rental_desc"_s).toString());
        field(qtTrId("student.rented_since"), m_data->formatDate(s.value(u"rental_since"_s).toString()));
    }
    field(qtTrId("common.notes"), s.value(u"notes"_s).toString());
    body += u"<h2>"_s + esc(qtTrId("student.tab.personal")) + u"</h2>"_s
            + table({}, personal);

    // Activities
    const QVariantList activities = m_data->rows(u"SELECT * FROM activity WHERE deleted = 0 AND student_id = ? "
                                                 u"ORDER BY date DESC, end_date DESC"_s, { studentId });
    if (!activities.isEmpty()) {
        QList<QStringList> regular, single;
        const QLocale locale = uiLocale();
        for (const QVariant &v : activities) {
            const QVariantMap a = v.toMap();
            if (a.value(u"continuous"_s).toInt() == 1) {
                const int weekday = a.value(u"weekday"_s).toInt();
                regular << QStringList { a.value(u"description"_s).toString(),
                                         weekday >= 0 && weekday < 7 ? locale.dayName(weekday + 1) : QString(),
                                         a.value(u"time"_s).toString(), m_data->formatDate(a.value(u"date"_s).toString()),
                                         m_data->formatDate(a.value(u"end_date"_s).toString()) };
            } else {
                const int kind = a.value(u"single_kind"_s).toInt();
                single << QStringList { a.value(u"description"_s).toString(),
                                        m_data->formatDate(a.value(u"date"_s).toString()),
                                        kind == 0 ? qtTrId("activity.type.solo")
                                        : kind == 1 ? qtTrId("activity.type.ensemble_participation")
                                                    : qtTrId("common.other") };
            }
        }
        body += u"<h2>"_s + esc(qtTrId("archive.doc.activities")) + u"</h2>"_s;
        if (!regular.isEmpty())
            body += u"<h3>"_s + esc(qtTrId("activity.regular")) + u"</h3>"_s
                    + table({ qtTrId("common.description"), qtTrId("common.weekday"), qtTrId("common.time"),
                              qtTrId("common.start"), qtTrId("common.end") }, regular);
        if (!single.isEmpty())
            body += u"<h3>"_s + esc(qtTrId("activity.irregular")) + u"</h3>"_s
                    + table({ qtTrId("common.description"), qtTrId("common.date"), qtTrId("common.type") }, single);
    }

    // Lessons with their notes and pieces: first the ended ones, then the ongoing
    const QString today = m_data->today();
    const QVariantList memberships = m_data->rows(
        u"SELECT m.*, l.name AS lesson_name FROM membership m LEFT JOIN lesson l ON l.id = m.lesson_id "
        u"WHERE m.deleted = 0 AND m.student_id = ? "
        u"ORDER BY CASE WHEN m.end_date <> '' AND m.end_date <= ? THEN 0 ELSE 1 END, m.start_date"_s,
        { studentId, today });
    if (!memberships.isEmpty()) {
        body += u"<h2>"_s + esc(qtTrId("archive.doc.notes_and_pieces")) + u"</h2><p>"_s
                + esc(qtTrId("archive.doc.lessons_taken")) + u":</p><ol>"_s;
        QStringList titles;
        for (const QVariant &v : memberships) {
            const QVariantMap m = v.toMap();
            const QString end = m.value(u"end_date"_s).toString();
            const bool ended = !end.isEmpty() && end <= today;
            const QString lessonName = ended && !m.value(u"previous_name"_s).toString().isEmpty()
                                           ? m.value(u"previous_name"_s).toString()
                                           : m.value(u"lesson_name"_s).toString();
            const QString period = ended ? qtTrId("common.from_to").arg(m_data->formatDate(m.value(u"start_date"_s).toString()),
                                                                         m_data->formatDate(end))
                                         : qtTrId("common.since").arg(m_data->formatDate(m.value(u"start_date"_s).toString()));
            titles << lessonName + u" – "_s + period;
            body += u"<li>"_s + esc(lessonName) + u" ("_s + esc(period) + u")</li>"_s;
        }
        body += u"</ol>"_s;
        for (int i = 0; i < memberships.size(); ++i) {
            const QString membership = memberships.at(i).toMap().value(u"id"_s).toString();
            QList<QStringList> notes;
            for (const QVariant &n : m_data->rows(u"SELECT date, content FROM note WHERE deleted = 0 AND membership_id = ? "
                                                  u"ORDER BY date"_s, { membership })) {
                const QVariantMap note = n.toMap();
                notes << QStringList { esc(m_data->formatDate(note.value(u"date"_s).toString())),
                                       m_data->noteHtml(note.value(u"content"_s).toString()) };
            }
            if (!notes.isEmpty())
                body += u"<h3>"_s + esc(qtTrId("common.lesson_notes") + u" – "_s + titles.at(i)) + u"</h3>"_s
                        + table({ qtTrId("common.date"), qtTrId("common.note") }, notes, false);
            QList<QStringList> pieces;
            for (const QVariant &p : m_data->rows(u"SELECT * FROM piece WHERE deleted = 0 AND membership_id = ? "
                                                  u"ORDER BY start_date"_s, { membership })) {
                const QVariantMap piece = p.toMap();
                const int minutes = piece.value(u"duration"_s).toInt();
                const QString composer = piece.value(u"composer"_s).toString();
                pieces << QStringList { (composer.isEmpty() ? QString() : composer + u": "_s) + piece.value(u"title"_s).toString(),
                                        piece.value(u"genre"_s).toString(),
                                        minutes > 0 ? u"%1 %2"_s.arg(minutes).arg(qtTrId("common.unit.min")) : QString(),
                                        m_data->formatDate(piece.value(u"start_date"_s).toString()),
                                        m_data->formatDate(piece.value(u"end_date"_s).toString()),
                                        pieceState(piece.value(u"state"_s).toInt()) };
            }
            if (!pieces.isEmpty())
                body += u"<h3>"_s + esc(qtTrId("common.pieces") + u" – "_s + titles.at(i)) + u"</h3>"_s
                        + table({ qtTrId("common.title"), qtTrId("common.genre"), qtTrId("common.duration"),
                                  qtTrId("common.start"), qtTrId("common.end"), qtTrId("common.status") }, pieces);
        }
    }
    return page(qtTrId("archive.entry") + u": "_s + name, body);
}

QString DocumentService::archiveStudent(const QString &studentId)
{
    const QVariantMap s = m_data->get(u"student"_s, studentId);
    if (s.isEmpty())
        return {};
    const QVariantMap fields { { u"forename"_s, s.value(u"forename"_s) }, { u"surname"_s, s.value(u"surname"_s) },
                               { u"content"_s, TextPack::pack(studentArchiveHtml(studentId)) },
                               { u"archived_on"_s, m_data->today() } };
    // The archive entry has the student's id: archiving again replaces it (as in Qupil 1.x),
    // also an entry that was deleted before
    if (!m_data->m_store->get(u"student_archive"_s, studentId).isEmpty()) {
        QVariantMap again = fields;
        again.insert(u"deleted"_s, 0);
        m_data->m_store->update(u"student_archive"_s, studentId, again);
        return studentId;
    }
    QVariantMap withId = fields;
    withId.insert(u"id"_s, studentId);
    return m_data->create(u"student_archive"_s, withId);
}
