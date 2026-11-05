// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "DemoData.h"

#include "Clock.h"
#include "Ids.h"
#include "Store.h"

#include <QDate>
#include <QHash>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QUuid>
#include <QVariantList>

#include <algorithm>
#include <array>
#include <iterator>

using namespace Qt::StringLiterals;

namespace Qupil::DemoData {

QStringList legacySchema()
{
    return {
        // revision 1 (mydbhandler.cpp)
        u"CREATE TABLE pupil (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, address TEXT, email TEXT, telefon TEXT, handy TEXT, birthday TEXT, notes TEXT, fathername TEXT, fatherjob TEXT, fathertelefon TEXT, mothername TEXT, motherjob TEXT, mothertelefon TEXT, firstlessondate TEXT, instrumenttype TEXT, instrumentsize TEXT, ifinstrumentnextsize INTEGER, ifrentinstrument INTEGER, rentinstrumentdesc TEXT, rentinstrumentstartdate TEXT)"_s,
        u"CREATE TABLE pupilarchive (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, data TEXT)"_s,
        u"CREATE TABLE lesson (lessonid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, state INTEGER, type INTEGER, autolessonname INTEGER, lessonname TEXT, unsteadylesson INTEGER, lessonday INTEGER, lessonstarttime TEXT, lessonstoptime TEXT, lessonlocation TEXT)"_s,
        u"CREATE TABLE lastlessonname ( llnid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonname TEXT)"_s,
        u"CREATE TABLE pupilatlesson (palid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonid INTEGER NOT NULL, pupilid INTEGER NOT NULL, llnid INTEGER, startdate TEXT, stopdate TEXT)"_s,
        u"CREATE TABLE piece (pieceid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cpieceid INTEGER, palid INTEGER, title TEXT, genre Text, duration INTEGER, startdate TEXT, stopdate TEXT, state INTEGER)"_s,
        u"CREATE TABLE note (noteid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cnoteid INTEGER, palid INTEGER, date TEXT, content TEXT)"_s,
        u"CREATE TABLE activity (activityid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, pupilid INTEGER, ifcontinous INTEGER, desc TEXT, continousday INTEGER, continoustime TEXT, date TEXT, continousstopdate TEXT)"_s,
        u"CREATE TABLE sheetmusiclibrary (smlid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author INTEGER, title TEXT, publisher INTEGER, rentpupilid INTEGER, lastrentdate TEXT)"_s,
        u"CREATE TABLE smlauthor (smlauthorid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author TEXT)"_s,
        u"CREATE TABLE smlpublisher (smlpublisherid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, publisher TEXT)"_s,
        // revision 2 (dbupdater.cpp)
        u"CREATE TABLE dbinfos (id INTEGER PRIMARY KEY NOT NULL, data_structure_rev INTEGER NOT NULL)"_s,
        u"REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,2 )"_s,
        u"ALTER TABLE activity ADD COLUMN noncontinoustype INTEGER NOT NULL DEFAULT 0"_s,
        u"ALTER TABLE pupil ADD COLUMN recitalinterval INTEGER NOT NULL DEFAULT 5"_s,
        u"CREATE TABLE piececomposer (piececomposerid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, composer TEXT)"_s,
        u"INSERT INTO piececomposer (composer) VALUES ('')"_s,
        u"ALTER TABLE piece ADD COLUMN piececomposerid INTEGER DEFAULT 1"_s,
        u"CREATE TABLE recital (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, date TEXT, time TEXT, location TEXT, organisator TEXT, defaultaccompanist TEXT, state INTEGER DEFAULT 0)"_s,
        u"CREATE TABLE recitalarchive (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, data TEXT)"_s,
        u"CREATE TABLE pieceatrecital (parid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, pieceid INTEGER NOT NULL, recitalid INTEGER NOT NULL, sorting INTEGER DEFAULT 0, ifexternalpiece INTEGER DEFAULT 0)"_s,
        u"CREATE TABLE externalrecitalpiece (erpid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, composer TEXT, title TEXT, genre TEXT, duration INTEGER, musician TEXT)"_s,
        // revision 3
        u"CREATE TABLE reminder (reminderid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, desc TEXT, mode INTEGER DEFAULT 0, pupilid INTEGER, notificationsound INTEGER DEFAULT 0)"_s,
        u"ALTER TABLE activity ADD COLUMN continoustype INTEGER NOT NULL DEFAULT 0"_s,
        u"UPDATE activity SET continousstopdate = '9999-99-99' WHERE continousstopdate IS NULL"_s,
        u"ALTER TABLE pupil ADD COLUMN ensembleactivityrequested INTEGER NOT NULL DEFAULT 1"_s,
        u"REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,3 )"_s,
    };
}

namespace {

constexpr auto kOpen = "9999-99-99";

struct Text
{
    bool de;
    QString operator()(const char *german, const char *english) const
    {
        return QString::fromUtf8(de ? german : english);
    }
};

//! A German and an English text
struct Pair
{
    const char *de;
    const char *en;
};

struct Student
{
    const char *forename;
    const char *surname;
    const char *instrumentDe;
    const char *instrumentEn;
    const char *size;
    int ageYears;
    int birthdayInDays;     // next birthday relative to today
    int learningYears;
};

// Invented people – any similarity is coincidental
const Student kStudentsDe[] = {
    { "Lena", "Muster", "Violine", "Violin", "3/4", 11, 3, 4 },
    { "Jonas", "Becker", "Viola", "Viola", "4/4", 14, 40, 6 },
    { "Mia", "Schneider", "Violine", "Violin", "1/2", 8, 120, 2 },
    { "Tom", "Wagner", "Cello", "Cello", "1/2", 9, 200, 3 },
    { "Emma", "Hoffmann", "Violine", "Violin", "4/4", 16, 65, 9 },
    { "Paul", "Richter", "Cello", "Cello", "4/4", 15, 15, 7 },
    { "Sophie", "Klein", "Violine", "Violin", "3/4", 10, 300, 3 },
    { "Ben", "Wolf", "Viola", "Viola", "4/4", 17, 250, 8 },
    { "Hannah", "Koch", "Kontrabass", "Double Bass", "3/4", 13, 90, 2 },
};
const Student kStudentsEn[] = {
    { "Lily", "Parker", "Violine", "Violin", "3/4", 11, 3, 4 },
    { "Noah", "Brooks", "Viola", "Viola", "4/4", 14, 40, 6 },
    { "Ava", "Turner", "Violine", "Violin", "1/2", 8, 120, 2 },
    { "Leo", "Hughes", "Cello", "Cello", "1/2", 9, 200, 3 },
    { "Grace", "Morgan", "Violine", "Violin", "4/4", 16, 65, 9 },
    { "Oliver", "Bennett", "Cello", "Cello", "4/4", 15, 15, 7 },
    { "Chloe", "Foster", "Violine", "Violin", "3/4", 10, 300, 3 },
    { "Jack", "Ellis", "Viola", "Viola", "4/4", 17, 250, 8 },
    { "Ella", "Price", "Kontrabass", "Double Bass", "3/4", 13, 90, 2 },
};

// Names for the students of the full week. The surnames sort after the first of the nine above
// (Becker / Bennett), so the screenshots keep their examples. Forenames are not those of the
// nine: the screenshot tour finds students by forename
const char *const kForenamesDe[] = {
    "Finn", "Lea", "Luis", "Clara", "Ida", "Elias", "Marie", "Leon", "Frieda", "Henry", "Mila", "Anton",
    "Lotta", "Jakob", "Nele", "Moritz", "Greta", "Theo", "Paula", "Oskar", "Romy", "Karl", "Lina", "Vincent",
    "Johanna", "Linus", "Matteo", "Carla", "Julian", "Alma", "David", "Pia", "Samuel", "Leonie", "Max", "Tilda",
    "Erik", "Merle", "Jan", "Amelie", "Niklas", "Isabel", "Till", "Luisa", "Jonathan", "Ronja", "Fabian",
    "Selin", "Aaron", "Zoe", "Simon", "Marlene", "Malte", "Juna", "Lasse", "Edda", "Ole", "Smilla", "Rafael",
    "Thea", "Mats", "Annika", "Levi", "Charlotte", "Bruno", "Elif", "Milan", "Hedda", "Yusuf", "Wilma",
};
const char *const kForenamesEn[] = {
    "Ethan", "Ruby", "Lucas", "Isla", "Mason", "Freya", "Harry", "Evie", "Arthur", "Poppy", "Alfie", "Daisy",
    "Henry", "Rosie", "Archie", "Millie", "Freddie", "Phoebe", "Oscar", "Matilda", "Jacob", "Sienna", "Charlie",
    "Florence", "George", "Willow", "Max", "Imogen", "Sam", "Lucy", "Toby", "Hazel", "Finley", "Erin", "Louis",
    "Holly", "Reuben", "Esme", "Dylan", "Maisie", "Harvey", "Megan", "Joseph", "Layla", "Elliot", "Martha",
    "Rory", "Beatrice", "Isaac", "Clara", "Frankie", "Ivy", "Zac", "Nancy", "Ollie", "Iris", "Ezra", "Thea",
    "Hugo", "Molly", "Kai", "Lola", "Seb", "Aria", "Jude", "Mabel", "Ronan", "Elsie", "Miles", "Nora",
};
const char *const kAdultsDe[] = { "Katrin", "Michael", "Sabine", "Andreas", "Birgit" };
const char *const kAdultsEn[] = { "Karen", "Michael", "Susan", "Andrew", "Helen" };
const char *const kSurnamesDe[] = {
    "Schulz", "Neumann", "Schwarz", "Zimmermann", "Braun", "Krüger", "Hofmann", "Hartmann", "Lange", "Schmitt",
    "Werner", "Krause", "Meier", "Lehmann", "Schmid", "Schulze", "Maier", "Köhler", "Herrmann", "König",
    "Walter", "Mayer", "Huber", "Kaiser", "Fuchs", "Peters", "Lang", "Scholz", "Möller", "Weiß", "Jung",
    "Hahn", "Vogel", "Friedrich", "Keller", "Günther", "Frank", "Berger", "Winkler", "Roth", "Lorenz",
    "Franke", "Ludwig", "Busch", "Böhm", "Kraft", "Jäger", "Otto", "Sommer", "Graf",
};
const char *const kSurnamesEn[] = {
    "Carter", "Davies", "Evans", "Fisher", "Graham", "Harris", "Irwin", "James", "Kelly", "Lewis", "Mason",
    "Nolan", "Owen", "Palmer", "Quinn", "Reed", "Shaw", "Taylor", "Walsh", "Young", "Cooper", "Dixon", "Ford",
    "Grant", "Hayes", "Hunt", "Jenkins", "King", "Lloyd", "Marsh", "Murray", "Norris", "Patel", "Rees",
    "Russell", "Scott", "Spencer", "Thomas", "Ward", "Watson", "Webb", "Wells", "West", "Wood", "Wright",
    "Bishop", "Brown", "Burke", "Chapman", "Doyle",
};

//! A student of the generated data
struct Person
{
    QString forename;
    QString surname;
    QString instrument;
    QString size;
    int ageYears = 10;
    int birthdayInDays = 100;
    int learningYears = 2;
};

//! How the notes of a lesson look: from one line to a whole practice programme in blocks
enum NoteStyle {
    Legacy,     // HTML as written by Qupil 1.x (the nine lessons of the first sample data)
    OneLine,    // a line per lesson
    TwoLines,   // two lines
    Growing,    // a few lines at first, then more and more blocks (a student who makes progress)
    LongBlocks, // short notes, the last one a whole practice programme in blocks
    LongList,   // short notes, the last one a long list without blocks ("Show more")
    Mixed,      // plain points and one block in between
    Class,      // a class at a primary school
    Group,      // a group lesson
    Quartet,    // a string quartet
};

struct Lesson
{
    int type;               // 1 single, 2 group, 3 ensemble
    int weekday;            // 0 = Monday … 4 = Friday; -1 = irregular
    const char *start;
    const char *end;
    int location;           // index into the location list
    QList<int> students;    // indexes into the students
    const char *manualName = nullptr;
    const char *manualEnglish = nullptr;
    bool piano = false;
    NoteStyle style = Legacy;
};

//! A lesson of the full week, with students made for it
struct Planned
{
    int type;
    int weekday;
    const char *start;
    const char *end;
    int location;
    char instrument;        // V violin, A viola, C cello, K piano
    int count;              // new students
    int age;                // of the first; adults from 18
    NoteStyle style;
    Pair name = { nullptr, nullptr };       // manual name (classes, quartet)
    QList<int> reuse = {};                  // students of other lessons who play here too
    Pair reminder = { nullptr, nullptr };   // "at the student", for the first one
    bool recentBirthday = false;            // the first had a birthday two days ago
};

// The week of a teacher of strings and piano: a string class at a primary school in the
// morning on some days, from 13:30 single lessons of 30 and 45 minutes, groups of three and
// four, the string ensemble on Tuesday and a quartet on Friday, until 18:45 … 20:00.
// The nine lessons of the first sample data stay where they were (Tuesday 14:00 Lena …).
const QList<Planned> kWeek {
    // Monday
    { 3, 0, "11:15", "12:00", 3, 'V', 5, 8, Class, { "Streicherklasse 3b", "String class 3b" } },
    { 1, 0, "13:30", "14:00", 0, 'V', 1, 9, OneLine },
    { 1, 0, "14:00", "14:30", 0, 'V', 1, 7, TwoLines },
    { 1, 0, "14:30", "15:00", 4, 'K', 1, 10, Growing, {}, {},
      { "Anmeldung zur Prüfung (Stufe 1) mitgeben", "Hand out the exam registration (level 1)" } },
    { 1, 0, "15:30", "16:15", 0, 'V', 1, 13, Growing },
    { 2, 0, "16:15", "17:00", 0, 'V', 3, 9, Group },
    { 1, 0, "17:00", "17:45", 4, 'K', 1, 15, TwoLines },
    { 1, 0, "17:45", "18:15", 0, 'C', 1, 11, OneLine, {}, {},
      { "Fotos für den Wettbewerb machen", "Take photos for the competition" } },
    { 1, 0, "18:15", "18:45", 0, 'V', 1, 14, Mixed, {}, {}, {}, true },
    { 1, 0, "18:45", "19:30", 4, 'K', 1, 42, LongBlocks },
    // Tuesday (the screenshots: 14:20, Lena's lesson is running)
    { 3, 1, "11:00", "11:45", 3, 'V', 5, 9, Class, { "Streicherklasse 4a", "String class 4a" } },
    { 1, 1, "13:30", "14:00", 4, 'K', 1, 8, OneLine, {}, {},
      { "Geliehene Noten zurückgeben lassen", "Ask for the borrowed music back" } },
    { 1, 1, "16:30", "17:00", 0, 'V', 1, 12, TwoLines, {}, {},
      { "Probe mit der Klavierbegleitung absprechen", "Arrange a rehearsal with the accompanist" }, true },
    { 1, 1, "18:30", "19:00", 0, 'A', 1, 15, Growing, {}, {},
      { "Anmeldebogen Musizierfreizeit einsammeln", "Collect the music camp form" } },
    { 1, 1, "19:00", "19:30", 4, 'K', 1, 38, OneLine },
    { 1, 1, "19:30", "20:00", 0, 'V', 1, 51, TwoLines },
    // Wednesday
    { 3, 2, "12:00", "12:45", 3, 'V', 5, 7, Class, { "Streicherklasse 2c", "String class 2c" } },
    { 1, 2, "13:30", "14:00", 0, 'V', 1, 8, OneLine },
    { 1, 2, "14:00", "14:30", 4, 'K', 1, 9, TwoLines, {}, {},
      { "Elternbrief zum Vorspiel mitgeben", "Hand out the recital letter for the parents" } },
    { 2, 2, "15:15", "16:00", 0, 'V', 3, 10, Group },
    { 1, 2, "16:00", "16:30", 4, 'K', 1, 12, Growing, {}, {}, {}, true },
    { 1, 2, "16:30", "17:00", 0, 'C', 1, 10, LongList, {}, {},
      { "Größeres Cello ausprobieren (3/4)", "Try a bigger cello (3/4)" } },
    { 1, 2, "17:00", "17:45", 0, 'V', 1, 16, Growing },
    { 2, 2, "17:45", "18:45", 0, 'V', 4, 12, Group },
    { 1, 2, "18:45", "19:15", 4, 'K', 1, 14, Mixed },
    // Thursday
    { 1, 3, "13:30", "14:15", 0, 'V', 1, 15, LongBlocks },
    { 1, 3, "14:15", "14:45", 4, 'K', 1, 8, OneLine },
    { 1, 3, "14:45", "15:15", 0, 'V', 1, 9, TwoLines, {}, {}, { "E-Saite wechseln", "Change the E string" } },
    { 2, 3, "15:15", "16:15", 1, 'C', 4, 11, Group },
    { 1, 3, "17:00", "17:30", 0, 'V', 1, 11, OneLine, {}, {}, {}, true },
    { 1, 3, "17:30", "18:15", 4, 'K', 1, 16, Growing },
    { 1, 3, "18:15", "18:45", 0, 'A', 1, 13, TwoLines, {}, {},
      { "Anmeldung „Jugend musiziert“ besprechen", "Discuss the competition registration" } },
    { 1, 3, "18:45", "19:15", 0, 'V', 1, 45, OneLine },
    // Friday
    { 3, 4, "11:00", "11:45", 3, 'V', 5, 8, Class, { "Streicherklasse 3a", "String class 3a" } },
    { 1, 4, "13:30", "14:00", 0, 'V', 1, 7, OneLine },
    { 1, 4, "14:00", "14:30", 4, 'K', 1, 11, Mixed, {}, {}, { "Übe-Tagebuch ansehen", "Look at the practice diary" } },
    { 2, 4, "14:30", "15:15", 0, 'V', 3, 10, Group, {}, {}, { "Duett-Stimmen kopieren", "Copy the duet parts" } },
    { 1, 4, "15:15", "15:45", 4, 'K', 1, 9, TwoLines },
    { 1, 4, "16:45", "17:15", 0, 'V', 1, 13, Growing, {}, {}, {}, true },
    { 1, 4, "17:15", "17:45", 0, 'C', 1, 14, OneLine },
    // Emma (4), a new violinist, Ben (7) and Paul (5)
    { 3, 4, "17:45", "18:45", 1, 'V', 1, 16, Quartet, { "Streichquartett", "String quartet" }, { 4, 7, 5 } },
};

// ------------------------------------------------------------------ note texts
// In the format of Qupil 2 (Qupil 1.x wrote HTML; such a text is kept as it is on import)

const Pair kOneLineStrings[] = {
    { "Tonleiter D-Dur, Menuett bis Takt 16", "D major scale, minuet up to bar 16" },
    { "Bogenhaltung: Daumen gebogen lassen", "Bow hold: keep the thumb bent" },
    { "Etüde Nr. 3 erst langsam, dann mit Metronom 72", "Study no. 3 slowly first, then with metronome 72" },
    { "Neues Stück: Gavotte – erste Zeile lesen", "New piece: gavotte – read the first line" },
    { "Saitenwechsel A–E sauber, ohne Nebengeräusche", "Clean string crossings A–E, no extra noise" },
    { "Vorspielstück zweimal am Tag durchspielen", "Play the recital piece through twice a day" },
    { "Lagenwechsel in die 3. Lage auf der A-Saite", "Shift to 3rd position on the A string" },
    { "**Intonation** im Menuett mit dem Stimmgerät prüfen", "Check the **intonation** of the minuet with the tuner" },
};
const Pair kOneLinePiano[] = {
    { "C-Dur-Tonleiter beidhändig, langsam", "C major scale hands together, slowly" },
    { "Menuett G-Dur: linke Hand allein üben", "Minuet in G: practise the left hand alone" },
    { "Pedal erst ab Takt 9", "Pedal only from bar 9" },
    { "Fingersatz in Takt 5–8 eintragen", "Write in the fingering for bars 5–8" },
    { "Für Elise: Anfang bis zur Wiederholung", "Für Elise: the opening up to the repeat" },
    { "Kadenz I–IV–V–I in G-Dur", "Cadence I–IV–V–I in G major" },
    { "Staccato rechts locker aus dem Handgelenk", "Right-hand staccato loosely from the wrist" },
    { "Neues Stück aus dem Album für die Jugend aussuchen", "Choose a new piece from the Album for the Young" },
};
const Pair kTwoLinesStrings[] = {
    { "Tonleiter G-Dur über zwei Oktaven\nKonzert: Takt 1–20 mit Bogeneinteilung",
      "G major scale over two octaves\nConcerto: bars 1–20 with bow distribution" },
    { "Etüde Nr. 5: Saitenwechsel langsam\nVibrato-Vorübung 5 Minuten am Tag",
      "Study no. 5: string crossings slowly\nVibrato exercise five minutes a day" },
    { "Gavotte auswendig bis zur Wiederholung\n**Intonation** in Takt 12 prüfen",
      "Gavotte by heart up to the repeat\nCheck the **intonation** in bar 12" },
    { "Lagenwechsel 1.–3. Lage\nNeues Stück: Noten besorgen",
      "Shifting 1st–3rd position\nNew piece: get the sheet music" },
    { "Lange Töne mit ganzem Bogen\n*Sehr schöner Ton heute!*",
      "Long notes with the whole bow\n*Lovely tone today!*" },
    { "Doppelgriffe mit leerer Saite\nTakt 9–16: ==crescendo erst ab Takt 13==",
      "Double stops with an open string\nBars 9–16: ==crescendo only from bar 13==" },
};
const Pair kTwoLinesPiano[] = {
    { "Sonatine C-Dur, 1. Satz: Takt 1–16 beidhändig\nTonleiter G-Dur mit Daumenuntersatz",
      "Sonatina in C, 1st movement: bars 1–16 hands together\nG major scale, thumb under" },
    { "Für Elise: Mittelteil linke Hand\n**Pedal** sauber wechseln",
      "Für Elise: middle section left hand\nChange the **pedal** cleanly" },
    { "Czerny-Etüde Nr. 4 mit Metronom 80\nBlattspiel: jeden Tag eine Zeile",
      "Czerny study no. 4 with metronome 80\nSight-reading: one line every day" },
    { "Präludium C-Dur: gleichmäßige Achtel\nDynamik: piano wirklich leise",
      "Prelude in C: even quavers\nDynamics: piano really soft" },
    { "Walzer: Bass auf eins tragen, Akkorde leicht\n*Schon sehr flüssig!*",
      "Waltz: carry the bass on the first beat, light chords\n*Already very fluent!*" },
    { "Akkorde in Umkehrungen, C- und G-Dur\nImprovisation über C–Am–F–G",
      "Chords in inversions, C and G major\nImprovise over C–Am–F–G" },
};

//! From the oldest to the most recent lesson: more and more structure
const Pair kGrowingStrings[] = {
    { "Tonleiter G-Dur über zwei Oktaven\nSeitz Nr. 2, 1. Satz: Takt 1–8 lesen",
      "G major scale over two octaves\nSeitz no. 2, 1st movement: read bars 1–8" },
    { "**Technik**\nTonleiter G-Dur, zwei Oktaven, ganze Bögen\nLeere Saiten: vom Frosch bis zur Spitze gleich laut\n\n"
      "**Seitz Nr. 2, 1. Satz**\nTakt 1–16, Bogenstriche eintragen",
      "**Technique**\nG major scale, two octaves, whole bows\nOpen strings: equally loud from frog to tip\n\n"
      "**Seitz no. 2, 1st movement**\nBars 1–16, write in the bowings" },
    { "**Technik**\nTonleiter G-Dur jetzt in Achteln, Metronom 60\nLeere Saiten gleich laut\n\n"
      "**Seitz Nr. 2, 1. Satz**\nTakt 1–16 sicher\nTakt 17–24 dazu, Sechzehntel langsam\n\n"
      "Noten für das Vorspiel kopieren",
      "**Technique**\nG major scale now in quavers, metronome 60\nOpen strings equally loud\n\n"
      "**Seitz no. 2, 1st movement**\nBars 1–16 secure\nAdd bars 17–24, semiquavers slowly\n\n"
      "Copy the music for the recital" },
    { "**Technik**\nTonleiter G-Dur, Metronom 72\nVibrato-Vorübung: Klopfen auf der A-Saite\n\n"
      "**Seitz Nr. 2, 1. Satz**\nTakt 1–32 im Zusammenhang\n==Takt 25: Lagenwechsel in die 3. Lage langsam==\n\n"
      "**Etüde**\nWohlfahrt Nr. 6, Zeile 1–3\n\n"
      "Vorspiel im Dezember: Seitz, 1. Satz",
      "**Technique**\nG major scale, metronome 72\nVibrato exercise: tapping on the A string\n\n"
      "**Seitz no. 2, 1st movement**\nBars 1–32 in one go\n==Bar 25: shift to 3rd position slowly==\n\n"
      "**Study**\nWohlfahrt no. 6, lines 1–3\n\n"
      "December recital: Seitz, 1st movement" },
    { "**Technik**\nTonleiter G-Dur, Metronom 80, auch in Terzen\nVibrato: jeden Tag 3 Minuten\n\n"
      "**Seitz Nr. 2, 1. Satz**\nGanzer Satz mit Klavierbegleitung (Aufnahme)\nTakt 25: Lagenwechsel klappt – jetzt im Tempo\n"
      "*Schluss: ritardando nicht zu früh*\n\n"
      "**Etüde**\nWohlfahrt Nr. 6 ganz\nSaitenwechsel aus dem Handgelenk\n\n"
      "**Vorspiel**\nAuftritt und Verbeugung üben\nZweimal am Stück vor der Familie spielen",
      "**Technique**\nG major scale, metronome 80, also in thirds\nVibrato: three minutes every day\n\n"
      "**Seitz no. 2, 1st movement**\nWhole movement with the piano recording\nBar 25: the shift works – now in tempo\n"
      "*Ending: don't start the ritardando too early*\n\n"
      "**Study**\nWohlfahrt no. 6 complete\nString crossings from the wrist\n\n"
      "**Recital**\nPractise walking on and bowing\nPlay it twice in a row for the family" },
};
const Pair kGrowingPiano[] = {
    { "Sonatine G-Dur: rechte Hand Takt 1–8\nTonleiter G-Dur rechte Hand",
      "Sonatina in G: right hand bars 1–8\nG major scale right hand" },
    { "**Sonatine G-Dur, 1. Satz**\nTakt 1–8 beidhändig, langsam\nTakt 9–16 rechte Hand\n\nTonleiter G-Dur beidhändig",
      "**Sonatina in G, 1st movement**\nBars 1–8 hands together, slowly\nBars 9–16 right hand\n\nG major scale hands together" },
    { "**Sonatine G-Dur, 1. Satz**\nTakt 1–16 beidhändig\nLinke Hand leiser als die Melodie\n\n"
      "**Technik**\nTonleiter G-Dur und e-Moll\nKadenz I–IV–V–I",
      "**Sonatina in G, 1st movement**\nBars 1–16 hands together\nLeft hand softer than the melody\n\n"
      "**Technique**\nG major and E minor scales\nCadence I–IV–V–I" },
    { "**Sonatine G-Dur, 1. Satz**\nGanzer Satz langsam beidhändig\n==Takt 12: Fingersatz 3-1 nehmen==\n\n"
      "**Bach, Präludium C-Dur**\nTakt 1–11 lesen, gleichmäßige Achtel\n\n"
      "**Technik**\nTonleiter G-Dur, e-Moll, Kadenz",
      "**Sonatina in G, 1st movement**\nWhole movement slowly, hands together\n==Bar 12: use fingering 3-1==\n\n"
      "**Bach, Prelude in C**\nRead bars 1–11, even quavers\n\n"
      "**Technique**\nG major, E minor, cadence" },
    { "**Sonatine G-Dur, 1. Satz**\nIm Tempo mit Metronom 92\nDynamik: piano wirklich leise\n\n"
      "**Bach, Präludium C-Dur**\nTakt 1–24\nPedal erst nach dem Akkordwechsel\n\n"
      "**Technik**\nTonleitern bis zwei Vorzeichen\nKadenz in allen geübten Tonarten\n\n"
      "Vorspiel: Sonatine, 1. Satz",
      "**Sonatina in G, 1st movement**\nIn tempo with metronome 92\nDynamics: piano really soft\n\n"
      "**Bach, Prelude in C**\nBars 1–24\nPedal only after the chord change\n\n"
      "**Technique**\nScales up to two sharps or flats\nCadence in every key practised\n\n"
      "Recital: sonatina, 1st movement" },
};

//! A whole practice programme (some teachers write them)
const Pair kLongPiano {
    "**Sitzhaltung**\nHocker so hoch, dass die Unterarme waagerecht sind\nSchultern locker, Ellbogen frei\n\n"
    "**Technik (jeden Tag 15 Minuten)**\n- Tonleitern H-Dur und gis-Moll, Hände getrennt, dann zusammen\n"
    "- Arpeggien über drei Oktaven\n- Czerny op. 299 Nr. 3: gleichmäßige Sechzehntel, Metronom 100\n"
    "- Akkordzerlegungen in Umkehrungen\n\n"
    "**Bach, Invention Nr. 8 F-Dur**\n- Takt 1–17 auswendig\n- Linke Hand allein: Phrasierung wie die rechte\n"
    "- ==Takt 12–14: Triller nicht zu früh beginnen==\n- Ohne Pedal!\n\n"
    "**Jazz: Autumn Leaves**\n- Linke Hand: Shell-Voicings (Terz und Septime)\n"
    "- Melodie mit leichtem Swing, Achtel nicht zu gerade\n- Improvisation nur mit Akkordtönen, zwei Chorusse\n\n"
    "**Chopin, Walzer a-Moll**\n- Takt 1–32\n- Bass auf eins tragen, Akkorde leicht\n- Rubato sparsam\n\n"
    "**Vorspiel im Dezember**\n- Reihenfolge: Bach, Chopin, Autumn Leaves\n- Einmal komplett vor Publikum (Familie) spielen\n\n"
    "*Lieber täglich 30 Minuten als einmal drei Stunden.*",
    "**Posture**\nStool high enough for level forearms\nShoulders loose, elbows free\n\n"
    "**Technique (15 minutes every day)**\n- B major and G sharp minor scales, hands separately, then together\n"
    "- Arpeggios over three octaves\n- Czerny op. 299 no. 3: even semiquavers, metronome 100\n"
    "- Broken chords in inversions\n\n"
    "**Bach, Invention no. 8 in F**\n- Bars 1–17 by heart\n- Left hand alone: phrase it like the right\n"
    "- ==Bars 12–14: don't start the trill too early==\n- No pedal!\n\n"
    "**Jazz: Autumn Leaves**\n- Left hand: shell voicings (third and seventh)\n"
    "- Melody with a light swing, quavers not too straight\n- Improvise with chord notes only, two choruses\n\n"
    "**Chopin, Waltz in A minor**\n- Bars 1–32\n- Carry the bass on the first beat, light chords\n- Little rubato\n\n"
    "**December recital**\n- Order: Bach, Chopin, Autumn Leaves\n- Play it all once for an audience (the family)\n\n"
    "*Better 30 minutes every day than three hours once.*"
};
const Pair kLongStrings {
    "**Haltung**\nGeige auf dem Schlüsselbein ablegen, nicht festklemmen\nBogenarm: Ellbogen auf Höhe der Saite\n\n"
    "**Jeden Tag (etwa 25 Minuten)**\n- Tonleiter A-Dur über zwei Oktaven, mit Metronom (66)\n"
    "- Dreiklänge A-Dur und fis-Moll\n- Kayser Nr. 9: Saitenwechsel erst langsam\n"
    "- Vibrato: zehnmal pro Finger, dann auf langen Tönen\n\n"
    "**Accolay, Konzert a-Moll**\n- Takt 1–40 mit Begleitung (Aufnahme)\n- Kadenz: Doppelgriffe einzeln stimmen\n"
    "- ==Takt 52: Lagenwechsel in die 5. Lage vorbereiten==\n\n"
    "**Kammermusik**\n- Zweite Stimme im Quartett: Einsätze zählen\n\n"
    "Wettbewerb: Anmeldeschluss beachten",
    "**Posture**\nRest the violin on the collarbone, don't clamp it\nBow arm: elbow at the height of the string\n\n"
    "**Every day (about 25 minutes)**\n- A major scale over two octaves, with metronome (66)\n"
    "- A major and F sharp minor arpeggios\n- Kayser no. 9: string crossings slowly first\n"
    "- Vibrato: ten times per finger, then on long notes\n\n"
    "**Accolay, Concerto in A minor**\n- Bars 1–40 with the accompaniment recording\n- Cadenza: tune the double stops one by one\n"
    "- ==Bar 52: prepare the shift to 5th position==\n\n"
    "**Chamber music**\n- Second part in the quartet: count the entries\n\n"
    "Competition: mind the registration deadline"
};
//! A long note without blocks: shortened with "Show more"
const Pair kLongList {
    "Tonleiter D-Dur über zwei Oktaven, ganze Bögen\nDanach in Achteln mit Metronom 66\n"
    "Leere Saiten C und G: ruhiger Bogen, Kontaktstelle halten\nDaumenlage vorbereiten: Flageolett in der Saitenmitte\n"
    "Dotzauer-Etüde Nr. 4: Zeile 1–4\nSaitenwechsel aus dem Handgelenk, Arm ruhig\n"
    "Bréval, Sonate C-Dur, 1. Satz: Takt 1–24\nTakt 9: vierter Finger statt leerer Saite\n"
    "Takt 17–20: crescendo bis zum forte\nDuett für das Vorspiel – erste Seite\n"
    "Stachel etwas länger ziehen, das Cello steht zu flach\n==Kolophonium mitbringen==",
    "D major scale over two octaves, whole bows\nThen in quavers with metronome 66\n"
    "Open C and G strings: steady bow, keep the contact point\nPrepare thumb position: harmonic in the middle of the string\n"
    "Dotzauer study no. 4: lines 1–4\nString crossings from the wrist, arm still\n"
    "Bréval, Sonata in C, 1st movement: bars 1–24\nBar 9: fourth finger instead of the open string\n"
    "Bars 17–20: crescendo up to forte\nDuet for the recital – first page\n"
    "Pull out the endpin a little, the cello is too flat\n==Bring rosin=="
};
//! Plain points with one block in between
const Pair kMixedStrings {
    "Lagenwechsel 1.–3. Lage auf der A-Saite\n\n**Vivaldi a-Moll, 3. Satz**\nTakt 1–20, Bogen an der Spitze\n"
    "Sechzehntel locker\n\nNeue Saiten aufziehen lassen",
    "Shifting 1st–3rd position on the A string\n\n**Vivaldi A minor, 3rd movement**\nBars 1–20, bow at the tip\n"
    "Loose semiquavers\n\nHave new strings put on"
};
const Pair kMixedPiano {
    "Tonleiter D-Dur beidhändig\n\n**Burgmüller, Arabeske**\nStaccato links leicht\nTakt 13–20 im Tempo\n\n"
    "Neues Stück für das Vorspiel aussuchen",
    "D major scale hands together\n\n**Burgmüller, Arabesque**\nLight staccato in the left hand\nBars 13–20 in tempo\n\n"
    "Choose a new piece for the recital"
};
const Pair kClassNotes[] = {
    { "Lied „Bruder Jakob“ als Kanon in zwei Gruppen\nPizzicato: Saiten benennen",
      "“Frère Jacques” as a round in two groups\nPizzicato: name the strings" },
    { "Bogenhaltung mit Bleistift üben", "Practise the bow hold with a pencil" },
    { "Rhythmusspiel: Ta und Ti-ti auf leeren Saiten", "Rhythm game: ta and ti-ti on open strings" },
    { "Noten lesen: d, e, fis auf der D-Saite\nInstrumente richtig einpacken",
      "Reading: D, E, F sharp on the D string\nPack the instruments properly" },
};
const Pair kGroupNotes[] = {
    { "Duett „Frühling“: Einsätze und Dynamik\nAufwärmen: Rhythmus-Kanon", "Duet “Spring”: entries and dynamics\nWarm-up: rhythm round" },
    { "Pizzicato-Polka: Takt 1–16 zusammen", "Pizzicato polka: bars 1–16 together" },
    { "Tonleiter im Kanon\nWeihnachtslied: Stimmen verteilen", "Scale as a round\nChristmas carol: hand out the parts" },
    { "Zusammen anfangen: Einsatz mit dem Atem\nBlattspiel: ein neues Duett",
      "Starting together: give the entry with a breath\nSight-reading: a new duet" },
};
const Pair kQuartetNotes[] = {
    { "**Mozart, Divertimento F-Dur KV 138, 1. Satz**\nTakt 1–30: Einsätze mit Atmen geben\nViola und Cello: Achtel nicht eilen\n\n"
      "Stimmen für das Vorspiel kopieren",
      "**Mozart, Divertimento in F, K. 138, 1st movement**\nBars 1–30: give the entries with a breath\nViola and cello: don't rush the quavers\n\n"
      "Copy the parts for the recital" },
    { "Mozart, 1. Satz: Takt 31–60\nIntonation der Akkorde in Takt 44 einzeln stimmen",
      "Mozart, 1st movement: bars 31–60\nTune the chords in bar 44 one by one" },
    { "Mozart, 1. Satz komplett", "Mozart, 1st movement complete" },
};

QString textFor(const Pair &p, bool de)
{
    return QString::fromUtf8(de ? p.de : p.en);
}

/*!
    The note of a lesson \a back weeks ago (0 = the most recent one). \a seed spreads the
    texts so that neighbouring lessons don't read the same
*/
QString noteText(NoteStyle style, bool piano, int back, int weeks, int seed, bool de)
{
    const int oldest = weeks - 1 - back;   // 0 for the first note
    switch (style) {
    case OneLine:
        return piano ? textFor(kOneLinePiano[(seed + back) % std::size(kOneLinePiano)], de)
                     : textFor(kOneLineStrings[(seed + back) % std::size(kOneLineStrings)], de);
    case TwoLines:
        return piano ? textFor(kTwoLinesPiano[(seed + back) % std::size(kTwoLinesPiano)], de)
                     : textFor(kTwoLinesStrings[(seed + back) % std::size(kTwoLinesStrings)], de);
    case Growing: {
        const int stage = std::clamp(oldest - (weeks - int(std::size(kGrowingStrings))), 0, int(std::size(kGrowingStrings)) - 1);
        return piano ? textFor(kGrowingPiano[stage], de) : textFor(kGrowingStrings[stage], de);
    }
    case LongBlocks:
        if (back == 0)
            return piano ? textFor(kLongPiano, de) : textFor(kLongStrings, de);
        return noteText(back % 2 ? TwoLines : OneLine, piano, back, weeks, seed, de);
    case LongList:
        return back == 0 ? textFor(kLongList, de) : noteText(TwoLines, piano, back, weeks, seed, de);
    case Mixed:
        return back == 0 ? (piano ? textFor(kMixedPiano, de) : textFor(kMixedStrings, de))
                         : noteText(OneLine, piano, back, weeks, seed, de);
    case Class:
        return textFor(kClassNotes[(seed + back) % std::size(kClassNotes)], de);
    case Group:
        return textFor(kGroupNotes[(seed + back) % std::size(kGroupNotes)], de);
    case Quartet:
        return textFor(kQuartetNotes[std::min(back, int(std::size(kQuartetNotes)) - 1)], de);
    case Legacy:
        break;
    }
    return {};
}

//! Plans for coming lessons (and two already held, not yet turned into a note)
struct PlanText
{
    int weekday;
    const char *start;
    bool held;          // the lesson has been held: "Make a note from it"
    Pair text;
};
const QList<PlanText> kPlans {
    { 0, "15:30", false, { "**Technik**\nDreiklänge G-Dur\n\n**Rieding op. 35**\nTakt 1–24 mit Begleitung\nSchluss auswendig",
                           "**Technique**\nG major arpeggios\n\n**Rieding op. 35**\nBars 1–24 with accompaniment\nEnding by heart" } },
    { 0, "18:45", false, { "Autumn Leaves: Shell-Voicings\nChopin, Walzer: Takt 17–32",
                           "Autumn Leaves: shell voicings\nChopin, waltz: bars 17–32" } },
    { 1, "13:30", true, { "Für Elise: Mittelteil\nC-Dur-Tonleiter beidhändig\nNeues Stück aussuchen",
                          "Für Elise: middle section\nC major scale hands together\nChoose a new piece" } },
    { 1, "16:30", false, { "Tonleiter A-Dur\nEtüde Nr. 7 vorspielen lassen\nNeues Stück: Accolay oder Küchler?",
                           "A major scale\nHear study no. 7\nNew piece: Accolay or Küchler?" } },
    { 1, "18:30", false, { "**Telemann, Konzert G-Dur**\nTakt 1–30 mit Begleitung\nTriller in Takt 14\n\nBlattspiel im Altschlüssel",
                           "**Telemann, Concerto in G**\nBars 1–30 with accompaniment\nTrill in bar 14\n\nSight-reading in the alto clef" } },
    { 2, "17:00", false, { "**Technik**\nTonleiter B-Dur in Terzen\nVibrato auf langen Tönen\n\n**Accolay**\nKadenz: Doppelgriffe\nTakt 40–60",
                           "**Technique**\nB flat major scale in thirds\nVibrato on long notes\n\n**Accolay**\nCadenza: double stops\nBars 40–60" } },
    { 2, "17:45", false, { "Aufwärmen: Tonleiter im Kanon\nDuett „Frühling“: Einsätze\nRhythmus im 6/8-Takt klatschen",
                           "Warm-up: scale as a round\nDuet “Spring”: entries\nClap the rhythm in 6/8" } },
    { 3, "13:30", false, { "**Accolay, Konzert a-Moll**\nTakt 40–60 dazunehmen\nKadenz mit Begleitung\n\n"
                           "**Etüde**\nKayser Nr. 10 anfangen\n\n**Quartett**\nZweite Stimme: Takt 31–60\n\nAnmeldung Wettbewerb ausfüllen",
                           "**Accolay, Concerto in A minor**\nAdd bars 40–60\nCadenza with accompaniment\n\n"
                           "**Study**\nStart Kayser no. 10\n\n**Quartet**\nSecond part: bars 31–60\n\nFill in the competition registration" } },
    { 3, "15:15", true, { "Bogenstrich-Spiel „Eisenbahn“\nLied „Der Kuckuck und der Esel“ zweistimmig\nNoten lesen im Bassschlüssel",
                          "Bowing game “railway”\nTwo-part song “The cuckoo and the donkey”\nReading in the bass clef" } },
    { 4, "11:00", false, { "Begrüßungslied\nBogenhaltung kontrollieren\nNeues Lied: „Jingle Bells“ auf der A-Saite",
                           "Welcome song\nCheck the bow hold\nNew song: “Jingle Bells” on the A string" } },
    { 4, "14:30", false, { "Weihnachtslied für das Vorspiel verteilen\nPizzicato-Polka: Einsätze",
                           "Hand out the Christmas carol for the recital\nPizzicato polka: entries" } },
    { 4, "15:15", false, { "Sonatine: Takt 1–16\nTonleiter F-Dur", "Sonatina: bars 1–16\nF major scale" } },
};

class Writer
{
public:
    explicit Writer(QSqlDatabase db) : m_db(db) {}

    qint64 insert(const QString &table, const QStringList &columns, const QVariantList &values)
    {
        QSqlQuery q(m_db);
        QStringList marks;
        for (int i = 0; i < columns.size(); ++i)
            marks << u"?"_s;
        q.prepare(u"INSERT INTO %1 (%2) VALUES (%3)"_s.arg(table, columns.join(u", "_s), marks.join(u", "_s)));
        for (const QVariant &v : values)
            q.addBindValue(v);
        if (!q.exec()) {
            if (m_error.isEmpty())
                m_error = q.lastError().text() + u" ("_s + table + u")"_s;
            return -1;
        }
        return q.lastInsertId().toLongLong();
    }
    void exec(const QString &sql)
    {
        QSqlQuery q(m_db);
        if (!q.exec(sql) && m_error.isEmpty())
            m_error = q.lastError().text() + u" ("_s + sql.left(60) + u")"_s;
    }
    QString error() const { return m_error; }

private:
    QSqlDatabase m_db;
    QString m_error;
};

QString iso(const QDate &d)
{
    return d.toString(Qt::ISODate);
}

//! Most recent date (today or before) that falls on the given weekday (0 = Monday)
QDate lastWeekday(const QDate &today, int weekday)
{
    return Clock::lastWeekday(today, weekday + 1);
}

//! The date, or the Friday before when it falls on a weekend: no lessons at weekends
QDate workday(const QDate &date)
{
    return date.dayOfWeek() > 5 ? lastWeekday(date, 4) : date;
}

//! The lesson at \a weekday / \a start that has begun last (today's once it has started)
QDate lastHeld(int weekday, const QString &start)
{
    const QDateTime now = Clock::now();
    if (now.date().dayOfWeek() == weekday + 1 && now.time() >= QTime::fromString(start, u"hh:mm"_s))
        return now.date();
    return lastWeekday(now.date().addDays(-1), weekday);
}

//! The next lesson at \a weekday / \a start that has not begun yet
QDate nextLesson(int weekday, const QString &start)
{
    const QDateTime now = Clock::now();
    QDate date = now.date();
    if (date.dayOfWeek() == weekday + 1 && now.time() >= QTime::fromString(start, u"hh:mm"_s))
        date = date.addDays(1);
    while (date.dayOfWeek() != weekday + 1)
        date = date.addDays(1);
    return date;
}

QString instrumentSize(char instrument, int age)
{
    switch (instrument) {
    case 'V':
        return age < 8 ? u"1/4"_s : age < 10 ? u"1/2"_s : age < 12 ? u"3/4"_s : u"4/4"_s;
    case 'C':
        return age < 10 ? u"1/2"_s : age < 13 ? u"3/4"_s : u"4/4"_s;
    case 'A':
        return u"4/4"_s;
    default:
        return {};
    }
}

QString instrumentName(char instrument, bool de)
{
    switch (instrument) {
    case 'A': return u"Viola"_s;
    case 'C': return u"Cello"_s;
    case 'K': return de ? u"Klavier"_s : u"Piano"_s;
    default: return de ? u"Violine"_s : u"Violin"_s;
    }
}

QString autoName(const Lesson &l, const QList<Person> &students, const QStringList &locations, bool de)
{
    QString name = l.type == 1 ? (de ? u"EU-"_s : u"IL-"_s)
                 : l.type == 2 ? (de ? u"GU-"_s : u"GL-"_s)
                               : (de ? u"EnsU-"_s : u"EnsL-"_s);
    if (l.weekday >= 0)
        name += QString::number(QTime::fromString(QLatin1StringView(l.start), u"hh:mm"_s)
                                    .secsTo(QTime::fromString(QLatin1StringView(l.end), u"hh:mm"_s)) / 60)
                + u"-"_s;
    name += locations.value(l.location).left(3) + u"-"_s;
    if (l.type == 1) {
        const Person &s = students[l.students.first()];
        name += s.forename.left(3) + s.surname.left(3);
    } else {
        QList<int> sorted = l.students;
        std::sort(sorted.begin(), sorted.end(), [&students](int a, int b) { return students[a].surname < students[b].surname; });
        for (int i : sorted)
            name += students[i].forename.left(1);
    }
    return name;
}

void writeConfig(const QString &path, const QStringList &locations, bool de)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    const Text t { de };
    QTextStream s(&f);
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<Qupil>\n    <Configuration>\n"
      << "        <ConfigRevision value=\"32\" />\n";
    auto list = [&s](const char *key, const char *tag, const QStringList &items) {
        s << "        <" << key << " value=\"" << tag << "\" type=\"list\">\n";
        for (const QString &item : items)
            s << "            <" << tag << " value=\"" << item.toHtmlEscaped() << "\" />\n";
        s << "        </" << key << ">\n";
    };
    list("PalPiecesGenreList", "PiecesGenre",
         { t("Lied", "Tune"), t("Barock", "Baroque"), t("Klassik", "Classical"), t("Romantik", "Romantic"),
           t("Moderne", "Modernism"), u"Irish Folk"_s, u"Pop"_s, u"Jazz"_s });
    list("LessonLocationList", "LessonLocation", locations);
    list("PupilInstrumentList", "Instrument",
         { t("Violine", "Violin"), u"Viola"_s, u"Cello"_s, t("Kontrabass", "Double Bass"), t("Klavier", "Piano") });
    list("PupilInstrumentSizeList", "InstrumentSize",
         { u"4/4"_s, u"3/4"_s, u"1/2"_s, u"1/4"_s, u"1/8"_s, u"1/16"_s });
    s << "        <BirthdayReminder value=\"1\" />\n"
      << "        <SaveNotesPiecesForAllPupil value=\"1\" />\n"
      << "        <RecitalModerationDuration value=\"4\" />\n"
      << "        <RecitalBetweenPiecesDuration value=\"2\" />\n"
      << "    </Configuration>\n</Qupil>\n";
}

QString plansPath(const QString &folder)
{
    return QDir(folder).filePath(u"plans.json"_s);
}

} // namespace

bool writeLegacyFolder(const QString &folder, const QString &language, QString *error)
{
    const bool de = language.startsWith(u"de"_s);
    const Text t { de };
    const QDate today = Clock::today();
    const QStringList locations { t("Raum 20", "Room 20"), t("Raum 16", "Room 16"), t("Aula", "Auditorium"),
                                  t("Grundschule Am Park", "Park Primary School"), t("Raum 12 (Flügel)", "Room 12 (grand piano)") };

    // ------------------------------------------------------------ the week
    // The nine students and lessons of the first sample data, then the full week
    QList<Person> students;
    for (const Student &s : de ? kStudentsDe : kStudentsEn)
        students << Person { QString::fromUtf8(s.forename), QString::fromUtf8(s.surname), t(s.instrumentDe, s.instrumentEn),
                             QString::fromLatin1(s.size), s.ageYears, s.birthdayInDays, s.learningYears };
    QList<Lesson> lessons {
        { 1, 1, "14:00", "14:45", 0, { 0 } },
        { 1, 1, "14:45", "15:15", 0, { 6 } },
        { 2, 1, "15:30", "16:30", 0, { 1, 2 } },
        { 3, 1, "17:00", "18:30", 2, { 0, 4, 5, 7, 8 }, "Streicherensemble", "String ensemble" },
        { 1, 0, "15:00", "15:30", 1, { 3 } },
        { 1, 4, "16:00", "16:45", 1, { 4 } },
        { 1, 2, "14:30", "15:15", 0, { 5 } },
        { 1, 3, "16:15", "17:00", 0, { 7 } },
        { 1, -1, "", "", 1, { 8 } },
    };
    const int firstLessons = lessons.size();
    QList<QPair<int, Pair>> studentReminders { { 3, { "Leihinstrument zur Kontrolle mitbringen lassen",
                                                      "Ask to bring the rental instrument for a check" } } };
    int child = 0;
    int adult = 0;
    int birthdayToday = -1;     // a student of a lesson later today
    for (const Planned &p : kWeek) {
        Lesson l { p.type, p.weekday, p.start, p.end, p.location, p.reuse, p.name.de, p.name.en, p.instrument == 'K', p.style };
        for (int k = 0; k < p.count; ++k) {
            const int age = p.age >= 18 ? p.age : p.age + (p.type == 3 ? k % 2 : k % 3);
            const int n = students.size();
            Person s;
            if (age >= 18) {
                s.forename = QString::fromUtf8(de ? kAdultsDe[adult % std::size(kAdultsDe)] : kAdultsEn[adult % std::size(kAdultsEn)]);
                ++adult;
            } else {
                s.forename = QString::fromUtf8(de ? kForenamesDe[child % std::size(kForenamesDe)]
                                                  : kForenamesEn[child % std::size(kForenamesEn)]);
                ++child;
            }
            s.surname = QString::fromUtf8(de ? kSurnamesDe[(n * 7) % std::size(kSurnamesDe)]
                                             : kSurnamesEn[(n * 7) % std::size(kSurnamesEn)]);
            s.instrument = instrumentName(p.instrument, de);
            s.size = instrumentSize(p.instrument, age);
            s.ageYears = age;
            // Birthdays spread over the year, none in the last days (except the ones wanted)
            s.birthdayInDays = k == 0 && p.recentBirthday ? -2 : 10 + (n * 53) % 340;
            s.learningYears = age >= 18 ? 3 + n % 5 : std::max(1, std::min(age - 6, 1 + n % 6));
            l.students << n;
            students << s;
            if (k == 0 && p.reminder.de)
                studentReminders << qMakePair(n, p.reminder);
        }
        // Today's birthday: a child with a lesson of its own today (the last one of the day)
        if (p.type == 1 && p.age < 18 && today.dayOfWeek() == p.weekday + 1 && !p.recentBirthday)
            birthdayToday = l.students.first();
        lessons << l;
    }
    if (birthdayToday >= 0)
        students[birthdayToday].birthdayInDays = 0;
    const int studentCount = students.size();

    QDir().mkpath(QDir(folder).filePath(u"db"_s));
    const QString path = QDir(folder).filePath(u"db/qupil.db"_s);
    QFile::remove(path);
    writeConfig(QDir(folder).filePath(u"config.xml"_s), locations, de);

    const QString connection = u"qupil-demo-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString failure;
    QSet<QPair<int, QString>> heldPlans;    // lessons whose last note is still a plan
    for (const PlanText &p : kPlans) {
        if (p.held)
            heldPlans.insert({ p.weekday, QString::fromLatin1(p.start) });
    }
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, connection);
        db.setDatabaseName(path);
        if (!db.open()) {
            failure = db.lastError().text();
        } else {
            Writer w(db);
            w.exec(u"BEGIN"_s);
            for (const QString &sql : legacySchema())
                w.exec(sql);

            // ------------------------------------------------------------ students
            QList<qint64> pupil;
            for (int i = 0; i < studentCount; ++i) {
                const Person &s = students[i];
                const QDate birthday = today.addDays(s.birthdayInDays).addYears(-s.ageYears);
                const bool rental = i == 0 || i == 3;
                const bool minor = s.ageYears < 18;
                pupil << w.insert(u"pupil"_s,
                    { u"surname"_s, u"forename"_s, u"address"_s, u"email"_s, u"telefon"_s, u"handy"_s, u"birthday"_s,
                      u"notes"_s, u"mothername"_s, u"mothertelefon"_s, u"firstlessondate"_s, u"instrumenttype"_s,
                      u"instrumentsize"_s, u"ifinstrumentnextsize"_s, u"ifrentinstrument"_s, u"rentinstrumentdesc"_s,
                      u"rentinstrumentstartdate"_s, u"recitalinterval"_s, u"ensembleactivityrequested"_s },
                    { s.surname, s.forename,
                      t("Musterweg %1, 12345 Notenstadt", "%1 Example Lane, Melody Town").arg(3 + i * 4),
                      s.forename.toLower() + u"@example.org"_s,
                      u"0123 45 67%1"_s.arg(10 + i), u"0170 000 00%1"_s.arg(10 + i), iso(birthday),
                      i == 0 ? t("Übt am liebsten morgens. Spielt gern Barock.", "Likes to practise in the morning. Loves baroque music.")
                             : QString(),
                      minor ? t("Mutter von %1", "Mother of %1").arg(s.forename) : QString(),
                      minor ? u"0123 98 76%1"_s.arg(i) : QString(),
                      iso(workday(today.addYears(-s.learningYears).addMonths(-1))), s.instrument,
                      s.size, i == 0 || i == 3 ? 1 : 0, rental ? 1 : 0,
                      rental ? t("Leihinstrument Nr. %1", "Rental instrument no. %1").arg(12 + i) : QString(),
                      rental ? iso(workday(today.addMonths(-14))) : QString(), i == 2 ? 3 : 5, i == 2 ? 0 : 1 });
            }
            w.insert(u"pupilarchive"_s, { u"surname"_s, u"forename"_s, u"data"_s },
                     { t("Früher", "Former"), t("Anna", "Amy"),
                       t("<h1>Archiveintrag: Anna Früher</h1><p>Unterricht von 2016 bis 2022.</p>",
                         "<h1>Archive entry: Amy Former</h1><p>Lessons from 2016 to 2022.</p>") });

            // ------------------------------------------------------------ lessons
            QList<QList<qint64>> memberships;   // per lesson: palid per student
            QList<QDate> lastLesson;            // per lesson: date of the most recent lesson
            for (const Lesson &l : lessons) {
                const QString name = l.manualName ? QString::fromUtf8(de || !l.manualEnglish ? l.manualName : l.manualEnglish)
                                                  : autoName(l, students, locations, de);
                const int weekday = l.weekday < 0 ? 0 : l.weekday;
                const qint64 lesson = w.insert(u"lesson"_s,
                    { u"state"_s, u"type"_s, u"autolessonname"_s, u"lessonname"_s, u"unsteadylesson"_s,
                      u"lessonday"_s, u"lessonstarttime"_s, u"lessonstoptime"_s, u"lessonlocation"_s },
                    { 1, l.type, l.manualName ? 0 : 1, name, l.weekday < 0 ? 1 : 0, weekday,
                      QString::fromLatin1(l.start), QString::fromLatin1(l.end), locations.value(l.location) });
                QList<qint64> pals;
                for (int s : l.students) {
                    pals << w.insert(u"pupilatlesson"_s, { u"lessonid"_s, u"pupilid"_s, u"startdate"_s, u"stopdate"_s },
                                     { lesson, pupil.value(s), iso(workday(today.addMonths(-13 - s % 12))), QString::fromLatin1(kOpen) });
                }
                memberships << pals;
                // Irregular: a Thursday the week before
                lastLesson << (l.weekday < 0 ? lastWeekday(today.addDays(-7), 3) : lastWeekday(today.addDays(-1), weekday));
            }
            // A former lesson of Tom / Leo, ended last summer
            const qint64 llnid = w.insert(u"lastlessonname"_s, { u"lessonname"_s }, { t("EU-30-Rau-TomWag", "IL-30-Roo-LeoHug") });
            const qint64 oldLesson = w.insert(u"lesson"_s,
                { u"state"_s, u"type"_s, u"autolessonname"_s, u"lessonname"_s, u"unsteadylesson"_s, u"lessonday"_s,
                  u"lessonstarttime"_s, u"lessonstoptime"_s, u"lessonlocation"_s },
                { 0, 1, 1, t("EU-30-Rau-TomWag", "IL-30-Roo-LeoHug"), 0, 3, u"15:00"_s, u"15:30"_s, locations[0] });
            const qint64 oldPal = w.insert(u"pupilatlesson"_s,
                { u"lessonid"_s, u"pupilid"_s, u"llnid"_s, u"startdate"_s, u"stopdate"_s },
                { oldLesson, pupil.value(3), llnid, iso(workday(today.addYears(-3))), iso(lastWeekday(today.addMonths(-4), 3)) });

            // ------------------------------------------------------------ notes
            // The nine lessons of the first sample data: HTML as Qupil 1.x wrote it
            const QStringList noteTexts {
                t("<p><b>G-Dur-Tonleiter</b> über zwei Oktaven, <span style=\"color:#d02020;\">langsam mit Metronom (60)</span>.</p><p>Bourrée: Takt 9–16 auswendig.</p>",
                  "<p><b>G major scale</b> over two octaves, <span style=\"color:#d02020;\">slowly with metronome (60)</span>.</p><p>Bourrée: bars 9–16 by heart.</p>"),
                t("<p>Bogeneinteilung im Mittelteil geklärt. <i>Sehr schöner Ton!</i></p>",
                  "<p>Sorted out the bow distribution in the middle section. <i>Lovely tone!</i></p>"),
                t("<p>Lagenwechsel 1.–3. Lage üben, jeden Tag 5 Minuten.</p>",
                  "<p>Shifting 1st–3rd position, five minutes every day.</p>"),
                t("<p>Neues Stück ausgesucht. Noten bis nächste Woche besorgen.</p>",
                  "<p>Picked a new piece. Get the sheet music by next week.</p>"),
                t("<p><b>Vibrato</b>: Handgelenk locker lassen. Etüde Nr. 4 vorbereiten.</p>",
                  "<p><b>Vibrato</b>: keep the wrist relaxed. Prepare study no. 4.</p>"),
            };
            for (int li = 0; li < lessons.size(); ++li) {
                const Lesson &l = lessons[li];
                const bool first = li < firstLessons;
                const int weeks = first ? (l.type == 3 ? 2 : 4) : l.type == 3 ? 3 : l.type == 2 ? 4 : 6;
                // A plan that was not turned into a note yet stands for the last lesson
                const bool planHeld = heldPlans.contains({ l.weekday, QString::fromLatin1(l.start) })
                                      && lastHeld(l.weekday, QString::fromLatin1(l.start)) == lastLesson[li];
                for (int k = planHeld ? 1 : 0; k < weeks; ++k) {
                    const QDate date = lastLesson[li].addDays(-7 * k);
                    const QString content = first ? noteTexts[(li + k) % noteTexts.size()]
                                                  : noteText(l.style, l.piano, k, weeks, li, de);
                    qint64 common = -1;
                    for (qint64 pal : memberships[li]) {
                        const qint64 id = w.insert(u"note"_s, { u"palid"_s, u"date"_s, u"content"_s },
                                                   { pal, iso(date), content });
                        if (common < 0)
                            common = id;
                        w.exec(u"UPDATE note SET cnoteid = %1 WHERE noteid = %2"_s.arg(common).arg(id));
                    }
                }
            }
            {
                const qint64 id = w.insert(u"note"_s, { u"palid"_s, u"date"_s, u"content"_s },
                                           { oldPal, iso(lastWeekday(today.addMonths(-4), 3)),
                                             t("<p>Letzte Stunde vor dem Wechsel zu Raum 16.</p>",
                                               "<p>Last lesson before moving to room 16.</p>") });
                w.exec(u"UPDATE note SET cnoteid = %1 WHERE noteid = %1"_s.arg(id));
            }

            // ------------------------------------------------------------ pieces
            struct Piece { const char *composer; const char *title; const char *genreDe; const char *genreEn; int duration; int state; };
            const QList<Piece> repertoire {
                { "J. S. Bach", "Bourrée (Suite Nr. 3)", "Barock", "Baroque", 3, 3 },
                { "A. Vivaldi", "Konzert a-Moll, 1. Satz", "Barock", "Baroque", 4, 1 },
                { "O. Rieding", "Concertino op. 35", "Romantik", "Romantic", 6, 1 },
                { "F. Seitz", "Schülerkonzert Nr. 2", "Romantik", "Romantic", 7, 2 },
                { "W. A. Mozart", "Menuett KV 2", "Klassik", "Classical", 2, 4 },
                { "Traditional", "Cripple Creek", "Irish Folk", "Irish Folk", 2, 3 },
                { "G. F. Händel", "Sonate F-Dur, 2. Satz", "Barock", "Baroque", 3, 0 },
                { "C. Saint-Saëns", "Der Schwan", "Romantik", "Romantic", 3, 3 },
            };
            const QList<Piece> pianoRepertoire {
                { "J. S. Bach", "Präludium C-Dur BWV 846", "Barock", "Baroque", 2, 1 },
                { "L. v. Beethoven", "Sonatine G-Dur Anh. 5, 1. Satz", "Klassik", "Classical", 3, 3 },
                { "R. Schumann", "Wilder Reiter op. 68 Nr. 8", "Romantik", "Romantic", 1, 3 },
                { "C. Czerny", "Etüde op. 599 Nr. 19", "Romantik", "Romantic", 1, 1 },
                { "F. Burgmüller", "Arabeske op. 100 Nr. 2", "Romantik", "Romantic", 1, 2 },
                { "F. Chopin", "Walzer a-Moll (posth.)", "Romantik", "Romantic", 3, 1 },
                { "J. Kosma", "Autumn Leaves", "Jazz", "Jazz", 4, 0 },
                { "S. Joplin", "The Entertainer", "Pop", "Pop", 4, 3 },
            };
            QHash<QString, qint64> composers;
            auto composerId = [&](const char *name) {
                const QString key = QString::fromUtf8(name);
                if (!composers.contains(key))
                    composers.insert(key, w.insert(u"piececomposer"_s, { u"composer"_s }, { key }));
                return composers.value(key);
            };
            QList<qint64> readyPieces;   // common ids of pieces ready for the next recital
            for (int li = 0; li < lessons.size(); ++li) {
                const QList<Piece> &list = lessons[li].piano ? pianoRepertoire : repertoire;
                const int count = lessons[li].type == 3 ? 2 : 3;
                for (int k = 0; k < count; ++k) {
                    const Piece &p = list[(li * 3 + k) % list.size()];
                    const int state = lessons[li].type == 3 ? 1 : p.state;
                    qint64 common = -1;
                    for (qint64 pal : memberships[li]) {
                        const qint64 id = w.insert(u"piece"_s,
                            { u"palid"_s, u"title"_s, u"genre"_s, u"duration"_s, u"startdate"_s, u"stopdate"_s,
                              u"state"_s, u"piececomposerid"_s },
                            { pal, QString::fromUtf8(p.title), t(p.genreDe, p.genreEn), p.duration,
                              iso(workday(today.addDays(-20 * (k + 1) - li % 40))),
                              state == 4 ? QVariant(iso(workday(today.addDays(-10)))) : QVariant(), state,
                              composerId(p.composer) });
                        if (common < 0)
                            common = id;
                        w.exec(u"UPDATE piece SET cpieceid = %1 WHERE pieceid = %2"_s.arg(common).arg(id));
                    }
                    if (state == 3 && readyPieces.size() < 5)
                        readyPieces << common;
                }
            }

            // ------------------------------------------------------------ activities
            const QDate pastRecital = today.addMonths(-4).addDays(-3);
            for (int i : { 0, 4, 5 }) {
                w.insert(u"activity"_s,
                    { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"continousday"_s, u"continoustime"_s, u"date"_s,
                      u"continousstopdate"_s, u"continoustype"_s, u"noncontinoustype"_s },
                    { pupil.value(i), 1, t("Jugendorchester", "Youth orchestra"), 3,
                      u"17:30"_s, iso(lastWeekday(today.addYears(-1), 3)), QString::fromLatin1(kOpen), 0, 0 });
            }
            w.insert(u"activity"_s,
                { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"continousday"_s, u"continoustime"_s, u"date"_s,
                  u"continousstopdate"_s, u"continoustype"_s, u"noncontinoustype"_s },
                { pupil.value(1), 1, t("Musiktheorie", "Music theory"), 2, u"18:00"_s, iso(lastWeekday(today.addMonths(-8), 2)),
                  QString::fromLatin1(kOpen), 1, 0 });
            for (int i : { 0, 1, 4 }) {
                w.insert(u"activity"_s,
                    { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"date"_s, u"noncontinoustype"_s },
                    { pupil.value(i), 0, t("Sommervorspiel, Aula: J. S. Bach - Bourrée", "Summer recital, Auditorium: J. S. Bach - Bourrée"),
                      iso(pastRecital), 0 });
            }
            w.insert(u"activity"_s, { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"date"_s, u"noncontinoustype"_s },
                     { pupil.value(4), 0, t("Wettbewerb „Jugend musiziert“", "Regional youth music competition"),
                       iso(today.addMonths(-7)), 2 });

            // ------------------------------------------------------------ library
            const QList<std::array<const char *, 3>> library {
                std::array<const char *, 3> { "F. Wohlfahrt", "60 Etüden op. 45", "Edition Beispiel" },
                { "J. Trott", "Melodious Double-Stops", "Example Music" },
                { "S. Suzuki", "Violin School, Vol. 2", "Example Music" },
                { "O. Rieding", "Concertino op. 35", "Edition Beispiel" },
                { "J. F. Mazas", "Etüden op. 36", "Edition Beispiel" },
                { "Diverse", "", "Example Music" },   // title depends on the language, see below
            };
            QHash<QString, qint64> authors, publishers;
            for (int k = 0; k < library.size(); ++k) {
                const QString author = QString::fromUtf8(library[k][0]);
                const QString publisher = QString::fromUtf8(library[k][2]);
                if (!authors.contains(author))
                    authors.insert(author, w.insert(u"smlauthor"_s, { u"author"_s }, { author }));
                if (!publishers.contains(publisher))
                    publishers.insert(publisher, w.insert(u"smlpublisher"_s, { u"publisher"_s }, { publisher }));
                const bool lent = k == 0 || k == 3;
                w.insert(u"sheetmusiclibrary"_s, { u"author"_s, u"title"_s, u"publisher"_s, u"rentpupilid"_s, u"lastrentdate"_s },
                         { authors.value(author), k == 5 ? t("Weihnachtslieder für Streicher", "Christmas carols for strings")
                                                         : QString::fromUtf8(library[k][1]),
                           publishers.value(publisher), lent ? pupil.value(k == 0 ? 0 : 6) : -1,
                           lent ? QVariant(iso(workday(today.addDays(-21 - k)))) : QVariant() });
            }

            // ------------------------------------------------------------ events
            const qint64 recital = w.insert(u"recital"_s,
                { u"desc"_s, u"date"_s, u"time"_s, u"location"_s, u"organisator"_s, u"defaultaccompanist"_s, u"state"_s },
                { t("Herbstvorspiel", "Autumn recital"), iso(today.addDays(18)), u"16:00"_s, locations[2],
                  t("Musikschule Notenstadt", "Melody Town music school"), t("Frau Flügel", "Ms Keys"), 0 });
            int position = 0;
            for (qint64 piece : readyPieces) {
                w.insert(u"pieceatrecital"_s, { u"pieceid"_s, u"recitalid"_s, u"sorting"_s, u"ifexternalpiece"_s },
                         { piece, recital, position++, 0 });
            }
            const qint64 external = w.insert(u"externalrecitalpiece"_s,
                { u"composer"_s, u"title"_s, u"genre"_s, u"duration"_s, u"musician"_s },
                { u"F. Schubert"_s, t("Ständchen", "Serenade"), t("Romantik", "Romantic"), 4,
                  t("Gast, Clara (15) - Gesang", "Guest, Clara (15) - Voice") });
            w.insert(u"pieceatrecital"_s, { u"pieceid"_s, u"recitalid"_s, u"sorting"_s, u"ifexternalpiece"_s },
                     { external, recital, position++, 1 });
            w.insert(u"recital"_s,
                { u"desc"_s, u"date"_s, u"time"_s, u"location"_s, u"organisator"_s, u"defaultaccompanist"_s, u"state"_s },
                { t("Sommervorspiel", "Summer recital"), iso(pastRecital), u"17:00"_s, locations[2],
                  t("Musikschule Notenstadt", "Melody Town music school"), t("Frau Flügel", "Ms Keys"), 1 });
            w.insert(u"recitalarchive"_s, { u"desc"_s, u"data"_s },
                     { iso(today.addYears(-1)) + u"_17:00 "_s + t("Frühlingsvorspiel", "Spring recital"),
                       t("<h1>Frühlingsvorspiel</h1><p>Programm mit 8 Beiträgen</p>",
                         "<h1>Spring recital</h1><p>Programme with 8 contributions</p>") });

            // ------------------------------------------------------------ reminders
            // At the start of Qupil: things that stand for weeks
            const Pair startup[] = {
                { "Noten für das Ensemble bestellen", "Order sheet music for the ensemble" },
                { "Anmeldung Musizierfreizeit: alle Eltern informieren", "Music camp registration: tell all parents" },
                { "Sommerkonzert: abfragen, wer mitspielt", "Summer concert: ask who wants to play" },
            };
            for (const Pair &r : startup) {
                w.insert(u"reminder"_s, { u"desc"_s, u"mode"_s, u"pupilid"_s, u"notificationsound"_s },
                         { textFor(r, de), 0, QVariant(), 0 });
            }
            // At a student: when the lesson begins (and in the notices until then)
            for (const auto &[student, text] : studentReminders) {
                w.insert(u"reminder"_s, { u"desc"_s, u"mode"_s, u"pupilid"_s, u"notificationsound"_s },
                         { textFor(text, de), 2, pupil.value(student), 1 });
            }

            w.exec(u"COMMIT"_s);
            failure = w.error();
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connection);

    // ------------------------------------------------------------ plans (Qupil 2 only)
    if (failure.isEmpty()) {
        QJsonArray plans;
        for (const PlanText &p : kPlans) {
            const QString start = QString::fromLatin1(p.start);
            plans.append(QJsonObject { { u"weekday"_s, p.weekday }, { u"start"_s, start },
                                       { u"date"_s, iso(p.held ? lastHeld(p.weekday, start) : nextLesson(p.weekday, start)) },
                                       { u"content"_s, textFor(p.text, de) } });
        }
        QFile f(plansPath(folder));
        if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate) || f.write(QJsonDocument(plans).toJson()) < 0)
            failure = f.errorString();
    }
    if (error)
        *error = failure;
    return failure.isEmpty();
}

int addPlans(const QString &folder, Store *store)
{
    QFile f(plansPath(folder));
    if (!store || !f.open(QIODevice::ReadOnly))
        return 0;
    int added = 0;
    store->beginBatch();
    for (const QJsonValue &v : QJsonDocument::fromJson(f.readAll()).array()) {
        const QJsonObject p = v.toObject();
        const auto lessons = store->select(u"lesson"_s, u"active = 1 AND irregular = 0 AND weekday = ? AND start_time = ?"_s,
                                           { p.value(u"weekday"_s).toInt(), p.value(u"start"_s).toString() });
        if (lessons.isEmpty())
            continue;
        // Groups and ensembles: one plan for everybody (a copy per student, like notes)
        const QString key = newId();
        for (const QVariantMap &m : store->select(u"membership"_s, u"lesson_id = ?"_s, { lessons.first().value(u"id"_s) })) {
            if (!store->create(u"plan"_s, { { u"membership_id"_s, m.value(u"id"_s) }, { u"group_key"_s, key },
                                            { u"date"_s, p.value(u"date"_s).toString() },
                                            { u"content"_s, p.value(u"content"_s).toString() } }).isEmpty())
                ++added;
        }
    }
    store->endBatch(true);
    return added;
}

} // namespace Qupil::DemoData
