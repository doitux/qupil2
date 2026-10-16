# -*- coding: utf-8 -*-
"""Qupil 2 string catalog.

Each entry has a stable ID (used with qsTrId()/qtTrId() in the code), clean
English (source language), French and Spanish. German is taken verbatim from
the Qupil 1.4.1 translation (ts/qupil_de.ts) via the old 1.4.1 English source
text ("old") and only technically normalized (see build.py). Entries without
"old" are new in Qupil 2 and carry their own German text.

French strings are written with plain spaces before : ? ! and inside « »;
build.py turns those into non-breaking spaces.
"""

CATALOG = []


def E(id, old=None, en=None, fr=None, es=None, de=None, pick=None, c=None,
      note=None, merged=(), src=None, email=False, code=None):
    CATALOG.append(dict(
        id=id, old=old, en=en, fr=fr, es=es, de=de, pick=pick, comment=c,
        note=note, merged=list(merged), src=src, email=email, code=code,
        num=isinstance(en, (list, tuple))))


# Strings from 1.4.1 that are intentionally not carried over to Qupil 2.
DROPPED = {
    "Qupil": "Produktname, wird nicht übersetzt",
    "Q&upil": "Produktname im Menü",
    "UTF-8": "technischer Begriff",
    "Latin-1": "technischer Begriff",
    "http://qupil.de": "URL",
    "1": "Platzhalter-Label aus dem Designer",
    "D": "Farbvorschau-Buchstabe (Einstellungen)",
    "L": "Farbvorschau-Buchstabe (Einstellungen)",
    "P": "Farbvorschau-Buchstabe (Einstellungen)",
    "&Jo": "leere Test-Aktion",
    "Einstellungen ...": "unbenutzte Aktion ohne Übersetzung",
    "Neuer Unterricht ...": "unbenutzte Aktion ohne Übersetzung",
    "sorting": "versteckte Spalte",
    "Sorting": "versteckte Spalte",
    "Ctrl+N, P": "Tastenkürzel (werden in Qupil 2 im Code definiert)",
    "Ctrl+N, L": "Tastenkürzel",
    "Ctrl+L": "Tastenkürzel",
    "Ctrl+D": "Tastenkürzel",
    "Ctrl+Shift+K": "Tastenkürzel",
    "Ctrl+Shift+F": "Tastenkürzel",
    "Ctrl+Shift+U": "Tastenkürzel",
    "Ctrl+Q": "Tastenkürzel",
    "Ctrl+M": "Tastenkürzel",
    "Ctrl+B": "Tastenkürzel",
    "Lessons: 0 - Pupils: 0 ": "Startwert, identisch mit schedule.stats",
    "Could not load Metronoms Sounds": "Metronom erzeugt Klicks in Qupil 2 selbst",
    "Error loading sound files: All sound files must have the same sample format.": "Metronom erzeugt Klicks in Qupil 2 selbst",
    "This version of Qupil uses a new format to store data. Qupil will now try to convert the old data into the new format. \nThis process may take several minutes.": "Konvertierung von Daten vor 1.0 entfällt (siehe migration.pre10)",
    "Convert old data - Qupil": "Konvertierung vor 1.0 entfällt",
    "Convert old data ...": "Konvertierung vor 1.0 entfällt",
    "Conversion of the old data into the new format successfully completed!": "Konvertierung vor 1.0 entfällt",
    "An error occurred while converting the old data. \nPlease contact the program author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> to fix this issue!": "Konvertierung vor 1.0 entfällt",
}

# ---------------------------------------------------------------- app
E("app.window_title", old="Qupil %1 - Lesson Management for Instrumental Teacher - qupil.de",
  en="Qupil %1 – Lesson management for music teachers",
  fr="Qupil %1 – Gestion des cours pour professeurs de musique",
  es="Qupil %1 – Gestión de clases para profesores de música",
  c="%1 = version number")
E("app.window_title_page",
  en="%1 – Qupil", de="%1 - Qupil", fr="%1 – Qupil", es="%1 – Qupil",
  c="Desktop window title; %1 = title of the current page or dialog.",
  note="Ersetzt das „ - Qupil“, das in 1.4.1 an jedem Dialogtitel hing")

# ---------------------------------------------------------------- common
E("common.ok", old="&OK", en="OK", fr="OK", es="Aceptar")
E("common.cancel", old="Cancel", en="Cancel", fr="Annuler", es="Cancelar")
E("common.yes", old="Yes", en="Yes", fr="Oui", es="Sí")
E("common.no", old="No", en="No", fr="Non", es="No")
E("common.save", old="&Save", en="Save", fr="Enregistrer", es="Guardar")
E("common.add", old="&Add", en="Add", fr="Ajouter", es="Añadir", merged=["Add"])
E("common.add_ellipsis", old="Add ...", en="Add…", fr="Ajouter…", es="Añadir…")
E("common.remove", old="&Remove", en="Remove", fr="Retirer", es="Quitar")
E("common.delete", old="Delete", en="Delete", fr="Supprimer", es="Eliminar")
E("common.edit", old="&Edit", en="Edit", fr="Modifier", es="Editar", merged=["Edit"],
  c="Used as menu title and as button label.")
E("common.print", old="&Print", en="Print", fr="Imprimer", es="Imprimir")
E("common.export_pdf", old="Export to &PDF", en="Export as PDF", fr="Exporter en PDF", es="Exportar a PDF")
E("common.export_file", old="Export File", en="Export file", fr="Exporter le fichier", es="Exportar archivo")
E("common.filter_pdf", old="PDF File (*.pdf)", en="PDF file (*.pdf)", fr="Fichier PDF (*.pdf)",
  es="Archivo PDF (*.pdf)", c="File dialog filter. Keep the pattern in parentheses unchanged.")
E("common.all", old="All", en="All", fr="Tous", es="Todos",
  c="Filter option, e.g. all lesson locations.", merged=["(code) alle"])
E("common.never", old="Never", en="Never", fr="Jamais", es="Nunca")
E("common.none", old="none", en="None", fr="Aucune", es="Ninguna",
  c="Shown when a student never took part in an ensemble.")
E("common.other", old="Other", en="Other", fr="Autre", es="Otro",
  c="Activity type.", merged=["(code) sonstiges"])
E("common.error", old="Error", en="Error", fr="Erreur", es="Error", merged=["Error - Qupil"])
E("common.dont_show_again", old="Do not show this message again.",
  en="Don’t show this message again", fr="Ne plus afficher ce message",
  es="No volver a mostrar este mensaje")
E("common.name", old="Name", en="Name", fr="Nom", es="Nombre")
E("common.description", old="Description", en="Description", fr="Description", es="Descripción")
E("common.label.description", old="Description:", en="Description:", fr="Description :", es="Descripción:")
E("common.date", old="Date", en="Date", fr="Date", es="Fecha")
E("common.label.date", old="Date:", en="Date:", fr="Date :", es="Fecha:")
E("common.time", old="Time", en="Time", fr="Heure", es="Hora", merged=["Time   "])
E("common.label.time", old="Time:", en="Time:", fr="Heure :", es="Hora:")
E("common.time_oclock", code=" Uhr", de="%1 Uhr", en="%1", fr="%1", es="%1 h",
  c="Clock time; %1 = formatted time such as 15:00. Add a unit only if your language uses one.",
  note="In 1.4.1 hartkodiert („ Uhr“ in recitaltabs.cpp)")
E("common.weekday", old="Weekday", en="Weekday", fr="Jour", es="Día de la semana")
E("common.label.weekday", old="Weekday:", en="Weekday:", fr="Jour :", es="Día de la semana:")
E("common.start", old="Start", en="Start", fr="Début", es="Inicio")
E("common.label.start", old="Start:", en="Start:", fr="Début :", es="Inicio:")
E("common.end", old="End", en="End", fr="Fin", es="Fin")
E("common.label.end", old="End:", en="End:", fr="Fin :", es="Fin:")
E("common.duration", old="Duration", en="Duration", fr="Durée", es="Duración")
E("common.label.duration", old="Duration:", en="Duration:", fr="Durée :", es="Duración:")
E("common.type", old="Type", en="Type", fr="Type", es="Tipo")
E("common.label.type", old="Type:", en="Type:", fr="Type :", es="Tipo:", merged=["Type: "])
E("common.status", old="State", en="Status", fr="Statut", es="Estado")
E("common.label.status", old="State:", en="Status:", fr="Statut :", es="Estado:")
E("common.location", old="Location", en="Location", fr="Lieu", es="Lugar")
E("common.composer", old="Composer", en="Composer", fr="Compositeur", es="Compositor")
E("common.label.composer", old="Composer:", en="Composer:", fr="Compositeur :", es="Compositor:")
E("common.title", old="Title", en="Title", fr="Titre", es="Título")
E("common.label.title", old="Title:", en="Title:", fr="Titre :", es="Título:")
E("common.genre", old="Genre", en="Genre", fr="Genre", es="Género", merged=["Genre               "])
E("common.label.genre", old="Genre:", en="Genre:", fr="Genre :", es="Género:")
E("common.instrument", old="Instrument", en="Instrument", fr="Instrument", es="Instrumento")
E("common.label.instrument", old="Instrument:", en="Instrument:", fr="Instrument :", es="Instrumento:")
E("common.instruments", old="Instruments", en="Instruments", fr="Instruments", es="Instrumentos")
E("common.size", old="Size", en="Size", fr="Taille", es="Tamaño",
  c="Instrument size, e.g. 3/4.")
E("common.label.size", old="Size:", en="Size:", fr="Taille :", es="Tamaño:")
E("common.count", old="Number", en="Count", fr="Nombre", es="Cantidad")
E("common.note", old="Note", en="Note", fr="Note", es="Nota", c="A lesson note (text), not a musical note.")
E("common.notes", old="Notes", en="Notes", fr="Notes", es="Notas")
E("common.label.notes", old="Notes:", en="Notes:", fr="Notes :", es="Notas:")
E("common.student", old="Pupil", en="Student", fr="Élève", es="Alumno", merged=["&Pupil"])
E("common.label.student", old="Pupil:", en="Student:", fr="Élève :", es="Alumno:")
E("common.lesson", old="Lesson", en="Lesson", fr="Cours", es="Clase", merged=["&Lesson"])
E("common.label.lesson", old="Lesson:", en="Lesson:", fr="Cours :", es="Clase:")
E("common.lesson_notes", old="&Lesson Notes", en="Lesson notes", fr="Notes de cours",
  es="Notas de clase", merged=["Lesson notes"])
E("common.pieces", old="&Music Pieces", en="Pieces", fr="Morceaux", es="Obras",
  merged=["Music Pieces", "Music pieces"], c="Pieces of music a student works on.")
E("common.piece", old="&Music Piece", en="Piece", fr="Morceau", es="Obra")
E("common.activities", old="Activities", en="Activities", fr="Activités", es="Actividades",
  merged=["&Activities"])
E("common.event", old="Event", en="Event", fr="Événement", es="Evento", merged=["&Event"])
E("common.events", old="Events", en="Events", fr="Événements", es="Eventos")
E("common.archive", old="&Archive", en="Archive", fr="Archives", es="Archivo")
E("common.first_name", old="First name", en="First name", fr="Prénom", es="Nombre",
  merged=["First Name"], c="Also used to detect matching columns in imported CSV files.")
E("common.last_name", old="Last name", en="Last name", fr="Nom", es="Apellidos",
  merged=["Last Name"], c="Also used to detect matching columns in imported CSV files.")
E("common.label.first_name", old="First Name:", en="First name:", fr="Prénom :", es="Nombre:")
E("common.label.last_name", old="Last Name:", en="Last name:", fr="Nom :", es="Apellidos:")
E("common.address", old="Address", en="Address", fr="Adresse", es="Dirección")
E("common.label.address", old="Address:", en="Address:", fr="Adresse :", es="Dirección:")
E("common.birthday", old="Birthday", en="Date of birth", fr="Date de naissance",
  es="Fecha de nacimiento", c="Also used to detect matching columns in imported CSV files.")
E("common.label.birthday", old="Birthday:", en="Date of birth:", fr="Date de naissance :",
  es="Fecha de nacimiento:")
E("common.street", old="Street", en="Street", fr="Rue", es="Calle")
E("common.label.street", old="Street:", en="Street:", fr="Rue :", es="Calle:")
E("common.zip", old="Zip Code", en="ZIP code", fr="Code postal", es="Código postal")
E("common.label.zip", old="Zip Code:", en="ZIP code:", fr="Code postal :", es="Código postal:")
E("common.city", old="City", en="City", fr="Ville", es="Ciudad")
E("common.label.city", old="City:", en="City:", fr="Ville :", es="Ciudad:")
E("common.phone", old="Phone", en="Phone", fr="Téléphone", es="Teléfono")
E("common.label.phone", old="Phone:", en="Phone:", fr="Téléphone :", es="Teléfono:")
E("common.mobile", old="Mobile", en="Mobile", fr="Portable", es="Móvil")
E("common.label.mobile", old="Mobile:", en="Mobile:", fr="Portable :", es="Móvil:")
E("common.email", old="E-Mail", en="Email", fr="E-mail", es="Correo electrónico")
E("common.label.email", old="E-Mail:", en="Email:", fr="E-mail :", es="Correo electrónico:")
E("common.age_years", old="Years", src="%n year(s)",
  en=["%n year", "%n years"], de=["%n Jahr", "%n Jahre"],
  fr=["%n an", "%n ans"], es=["%n año", "%n años"],
  c="Age or duration in years.",
  note="1.4.1: Zahl + „Jahre“; jetzt mit Einzahl für 1")
E("common.duration_months", old="1 month", src="%n month(s)",
  en=["%n month", "%n months"], de=["%n Monat", "%n Monate"],
  fr=["%n mois", "%n mois"], es=["%n mes", "%n meses"],
  merged=["2 months", "3 months", "4 months", "6 months", "9 months", "12 months",
          "18 months", "24 months"],
  c="Used for the personal recital interval (1, 2, 3, 4, 6, 9, 12, 18, 24 months).",
  note="Die neun festen Einträge „1 Monat“ … „24 Monate“ als ein Pluraleintrag")
E("common.unit.min", old="Min.", en="min", fr="min", es="min", merged=[" Min."],
  c="Abbreviation for minutes, shown after a number.")
E("common.unit.minutes", old=" Minutes", en="minutes", fr="minutes", es="minutos")
E("common.unit.cm", old=" cm", en="cm", fr="cm", es="cm")
E("common.irregular", old="Irregular", en="Irregular", fr="Irrégulier", es="Irregular",
  merged=["irregular"], pick="Unregelmäßig",
  c="Used like a weekday for lessons without a fixed time.",
  note="1.4.1 hatte zusätzlich ein kleingeschriebenes „unregelmäßig“ im Stundenplan-Dokument")
E("common.from_to", old="from", en="from %1 to %2", de="vom %1 bis %2",
  fr="du %1 au %2", es="del %1 al %2", merged=["to"], c="%1, %2 = dates",
  note="Aus den Fragmenten „vom“ und „bis“ zusammengesetzt")
E("common.since", old="since", en="since %1", de="seit %1", fr="depuis le %1", es="desde el %1",
  c="%1 = date", note="Fragment „seit“ mit Platzhalter")
E("common.total_paren", old="total", en="(%1 in total)", de="(gesamt %1)", fr="(%1 au total)",
  es="(%1 en total)", c="%1 = a duration such as '7 years'",
  note="Fragment „gesamt“ mit Platzhalter")

# ---------------------------------------------------------------- weekdays
E("weekday.monday", old="Monday", en="Monday", fr="Lundi", es="Lunes")
E("weekday.tuesday", old="Tuesday", en="Tuesday", fr="Mardi", es="Martes")
E("weekday.wednesday", old="Wednesday", en="Wednesday", fr="Mercredi", es="Miércoles")
E("weekday.thursday", old="Thursday", en="Thursday", fr="Jeudi", es="Jueves")
E("weekday.friday", old="Friday", en="Friday", fr="Vendredi", es="Viernes")
E("weekday.saturday", old="Saturday", en="Saturday", fr="Samedi", es="Sábado")
E("weekday.sunday", old="Sunday", en="Sunday", fr="Dimanche", es="Domingo")

# ---------------------------------------------------------------- desktop menus
E("menu.new", old="&New ...", en="New…", fr="Nouveau…", es="Nuevo…")
E("menu.document", old="&Document", en="Document", fr="Document", es="Documento")
E("menu.tools", old="&Tools", en="Tools", fr="Outils", es="Herramientas")
E("menu.extras", old="&Extras", en="Extras", fr="Extras", es="Extras")
E("menu.quit", old="&Quit", en="Quit", fr="Quitter", es="Salir")
E("menu.configure", old="&Configure Qupil ...", en="Settings…", fr="Paramètres…", es="Ajustes…")
E("menu.event_manager", old="&Event Manager ...", en="Event manager…",
  fr="Gestionnaire d’événements…", es="Gestor de eventos…")
E("menu.instrument_manager", old="&Instrument manager ...", en="Instrument manager…",
  fr="Gestionnaire d’instruments…", es="Gestor de instrumentos…")
E("menu.library", old="&Music Library ...", en="Sheet music library…",
  fr="Bibliothèque de partitions…", es="Biblioteca de partituras…")
E("menu.about", old="&About Qupil ...", en="About Qupil…", fr="À propos de Qupil…", es="Acerca de Qupil…")
E("menu.metronome", old="&Metronome / Tuner ...", en="Metronome / tuning…",
  fr="Métronome / accordage…", es="Metrónomo / afinación…")
E("menu.reminders", old="&Reminder ...", en="Reminders…", fr="Rappels…", es="Recordatorios…")
E("menu.birthdays", old="&Show Birthdays", en="Show birthdays", fr="Afficher les anniversaires",
  es="Mostrar cumpleaños")
E("menu.backup_create", old="&Create Backup ...", en="Create backup…", fr="Créer une sauvegarde…",
  es="Crear copia de seguridad…")
E("menu.backup_restore", old="&Replay Backup...", en="Restore backup…",
  fr="Restaurer une sauvegarde…", es="Restaurar copia de seguridad…")
E("menu.import_csv", old="Import (CVS) &Address Book ...", en="Import address book (CSV)…",
  fr="Importer un carnet d’adresses (CSV)…", es="Importar libreta de direcciones (CSV)…")
E("menu.schedule_doc", old="&Timetable Document ...", en="Schedule document…",
  fr="Document de l’emploi du temps…", es="Documento de horario…")
E("menu.recital_candidates", old="&Concert Candidates ...", en="Recital candidates…",
  fr="Candidats aux auditions…", es="Candidatos a audición…")
E("menu.ensemble_candidates", old="&Ensemble Candidates ...", en="Ensemble candidates…",
  fr="Candidats aux ensembles…", es="Candidatos a conjunto…")
E("menu.dayview", old="&Daily Schedule ...", en="Daily overview…", fr="Aperçu du jour…",
  es="Resumen del día…")

# ---------------------------------------------------------------- navigation
E("nav.schedule", old="Timetable", en="Schedule", fr="Emploi du temps", es="Horario",
  c="Weekly lesson schedule (timetable).")
E("nav.student_list", old="Pupil List", en="Student list", fr="Liste des élèves", es="Lista de alumnos")
E("nav.student_archive", old="Pupil Archive", en="Student archive", fr="Archives des élèves",
  es="Archivo de alumnos")
E("nav.today", en="Today", de="Heute", fr="Aujourd’hui", es="Hoy")
E("nav.students", en="Students", de="Schüler", fr="Élèves", es="Alumnos")
E("nav.more", en="More", de="Mehr", fr="Plus", es="Más", c="Tab that lists all further tools.")
E("nav.candidates", en="Candidates", de="Kandidaten", fr="Candidats", es="Candidatos",
  c="Short label for recital and ensemble candidates.")
E("nav.events", en="Events", de="Veranstaltungen", fr="Événements", es="Eventos",
  c="Tab with recitals, concerts and other events.")
E("app.under_construction", en="This area is being built for Qupil 2.",
  de="Dieser Bereich entsteht gerade für Qupil 2.",
  fr="Cette partie de Qupil 2 est en cours de développement.",
  es="Esta sección de Qupil 2 está en desarrollo.",
  c="Placeholder on pages of the rewrite that are not finished yet.")

# ---------------------------------------------------------------- today (new)
E("today.notices", en="Notices", de="Hinweise", fr="À noter", es="Avisos")
E("today.later", en="Later today", de="Später heute", fr="Plus tard aujourd’hui", es="Más tarde hoy")
E("today.remaining", src="%n min left",
  en=["%n min left", "%n min left"], de=["noch %n Min.", "noch %n Min."],
  fr=["encore %n min", "encore %n min"], es=["queda %n min", "quedan %n min"],
  c="Remaining time of the running lesson.")
E("today.show_irregular", en="Show irregular lessons (%1)", de="Unregelmäßige Termine (%1) anzeigen",
  fr="Afficher les cours irréguliers (%1)", es="Mostrar clases irregulares (%1)",
  c="%1 = number of lessons")
E("today.birthday", en="Birthday today", de="Geburtstag heute", fr="Anniversaire aujourd’hui",
  es="Cumpleaños hoy")
E("today.birthday_turns", en="%1 turns %2", de="%1 wird %2", fr="%1 fête ses %2 ans",
  es="%1 cumple %2", c="%1 = student name, %2 = new age")
E("today.reminder_at", en="Reminder · %1", de="Erinnerung · %1", fr="Rappel · %1",
  es="Recordatorio · %1", c="%1 = time")
E("today.overdue", src="%n student(s) overdue",
  en=["%n student overdue", "%n students overdue"], de=["%n Schüler überfällig", "%n Schüler überfällig"],
  fr=["%n élève en retard", "%n élèves en retard"], es=["%n alumno pendiente", "%n alumnos pendientes"],
  c="Number of overdue recital candidates.")
E("today.no_lessons", en="No lessons today", de="Heute kein Unterricht", fr="Pas de cours aujourd’hui",
  es="Hoy no hay clases")

# ---------------------------------------------------------------- general actions (new)
E("common.new_item", en="New", de="Neu", fr="Nouveau", es="Nuevo", c="Button that creates a student or lesson.")
E("common.more_actions", en="More actions", de="Weitere Aktionen", fr="Plus d’actions", es="Más acciones")
E("common.done", en="Done", de="Fertig", fr="Terminé", es="Listo")
E("common.back", en="Back", de="Zurück", fr="Retour", es="Atrás")
E("common.close", en="Close", de="Schließen", fr="Fermer", es="Cerrar")
E("common.share", en="Share", de="Teilen", fr="Partager", es="Compartir")
E("common.undo", en="Undo", de="Rückgängig", fr="Annuler", es="Deshacer")
E("common.search", en="Search", de="Suchen", fr="Rechercher", es="Buscar")

# ---------------------------------------------------------------- schedule
E("schedule.stats", old="Lessons: %1 - Pupils: %2 ", en="Lessons: %1 – Students: %2",
  fr="Cours : %1 – Élèves : %2", es="Clases: %1 – Alumnos: %2", c="%1, %2 = numbers")
E("schedule.irregular_dates", old="irregular dates", en="Irregular dates", fr="Dates irrégulières",
  es="Fechas irregulares")
E("schedule.as_of", old="Status: ", en="As of:", fr="État au :", es="A fecha de:",
  c="Precedes the date a document was created.")

# ---------------------------------------------------------------- lessons
E("lesson.tab.details", old="Details", en="Details", fr="Détails", es="Detalles")
E("lesson.label.type", old="Lesson Type:", en="Lesson type:", fr="Type de cours :", es="Tipo de clase:")
E("lesson.type.individual", old="Individual Lesson", en="Individual lesson", fr="Cours individuel",
  es="Clase individual")
E("lesson.type.group", old="Group Lesson", en="Group lesson", fr="Cours collectif", es="Clase grupal")
E("lesson.type.ensemble", old="Ensemble Lesson", en="Ensemble lesson", fr="Cours d’ensemble",
  es="Clase de conjunto")
E("lesson.name.heading", old="Lesson Description", en="Lesson name", fr="Nom du cours",
  es="Nombre de la clase")
E("lesson.name.auto", old="Create Automatically:", en="Generate automatically:",
  fr="Générer automatiquement :", es="Generar automáticamente:")
E("lesson.name.manual", old="Manually:", en="Manually:", fr="Manuellement :", es="Manualmente:")
E("lesson.label.time", old="Lesson Time:", en="Lesson time:", fr="Horaire du cours :",
  es="Horario de la clase:")
E("lesson.time.fixed", old="Fixed Time:", en="Fixed time:", fr="Horaire fixe :", es="Horario fijo:")
E("lesson.time.irregular", old="Irregular Lesson", en="Irregular lesson", fr="Cours irrégulier",
  es="Clase irregular")
E("lesson.label.location", old="Location:", pick="Unterrichtsort:", en="Location:",
  fr="Lieu du cours :", es="Lugar de la clase:")
E("lesson.label.students", old="Pupils:", en="Students:", fr="Élèves :", es="Alumnos:")
E("lesson.label.students_in", old="Pupils in Lesson:", en="Students in this lesson:",
  fr="Élèves du cours :", es="Alumnos de la clase:")
E("lesson.label.available", old="Available Pupils:", en="Available students:",
  fr="Élèves disponibles :", es="Alumnos disponibles:")
E("lesson.delete", old="Delete &Lesson", en="Delete lesson", fr="Supprimer le cours",
  es="Eliminar clase", merged=["Delete Lesson", "Delete Lesson - Qupil"])
E("lesson.delete.confirm", old="Do you really want to delete the selected lesson?",
  en="Do you really want to delete the selected lesson?",
  fr="Voulez-vous vraiment supprimer le cours sélectionné ?",
  es="¿Seguro que desea eliminar la clase seleccionada?")
E("lesson.individual_one_student",
  old="You can not assign more than one student to an individual lesson \nPlease change the number of students to save!",
  en="An individual lesson can only have one student. Please change the number of students to save.",
  fr="Un cours individuel ne peut compter qu’un seul élève. Veuillez modifier le nombre d’élèves pour enregistrer.",
  es="Una clase individual solo puede tener un alumno. Cambie el número de alumnos para guardar.")
E("lesson.prefix.individual", old="IL-", en="IL-", fr="CI-", es="CI-",
  c="Abbreviation at the start of automatically generated names of individual lessons.")
E("lesson.prefix.group", old="GL-", en="GL-", fr="CC-", es="CG-",
  c="Abbreviation at the start of automatically generated names of group lessons.")
E("lesson.prefix.ensemble", old="EnsL-", en="EnsL-", fr="CE-", es="CC-",
  c="Abbreviation at the start of automatically generated names of ensemble lessons.")
E("lesson.new_name", code="EU - neu", de="EU - neu", en="IL – new", fr="CI – nouveau",
  es="CI – nueva", c="Name of a newly created lesson before it is configured.",
  note="In 1.4.1 hartkodiert (mainwindowimpl.cpp)")
E("lesson.inactive_suffix", old="(inactive)", en="(inactive)", fr="(inactif)", es="(inactiva)",
  c="Appended to the name of a lesson the student no longer attends.")
E("lesson.choose", en="Choose lesson", de="Unterricht wählen", fr="Choisir le cours", es="Elegir clase")

# ---------------------------------------------------------------- students
E("student.new.first_name", old="New", en="New", fr="Nouvel", es="Nuevo",
  c="First name of a newly created student; the last name is the word 'Student' (common.student), giving e.g. 'New Student'.")
E("student.delete", old="Delete &Pupil", en="Delete student", fr="Supprimer l’élève",
  es="Eliminar alumno", merged=["Delete Pupil"])
E("student.delete.confirm", old="Do you really want to delete the pupil \"%1\" ?",
  en="Do you really want to delete the student “%1”?",
  fr="Voulez-vous vraiment supprimer l’élève « %1 » ?",
  es="¿Seguro que desea eliminar al alumno “%1”?", c="%1 = student name")
E("student.archive", old="&Archive Pupil", en="Archive student", fr="Archiver l’élève",
  es="Archivar alumno", merged=["Archive Pupil"])
E("student.archive_delete", old="Archive and &Delete Pupil", en="Archive and delete student",
  fr="Archiver et supprimer l’élève", es="Archivar y eliminar alumno",
  merged=["Archive and Delete Pupil"])
E("student.tab.personal", old="&Personal Data", en="Personal data", fr="Données personnelles",
  es="Datos personales", merged=["Personal data"])
E("student.tab.loans", old="&Loaned Music", en="Borrowed sheet music", fr="Partitions empruntées",
  es="Partituras prestadas")
E("student.tab.data_short", en="Data", de="Daten", fr="Données", es="Datos",
  c="Short tab label for personal data on phones.")
E("student.tab.loans_short", en="Loans", de="Leihnoten", fr="Prêts", es="Préstamos",
  c="Short tab label for borrowed sheet music on phones.")
E("student.search", en="Search students", de="Schüler suchen", fr="Rechercher des élèves",
  es="Buscar alumnos")
E("student.label.age", old="Age:", en="Age:", fr="Âge :", es="Edad:")
E("student.label.parents", old="Parents:", en="Parents:", fr="Parents :", es="Padres:")
E("student.label.mother_name", old="Name (Mother):", en="Mother’s name:", fr="Nom de la mère :",
  es="Nombre de la madre:")
E("student.label.mother_job", old="Job (Mother):", en="Mother’s occupation:",
  fr="Profession de la mère :", es="Profesión de la madre:")
E("student.label.mother_phone", old="Phone (Mother):", en="Mother’s phone:",
  fr="Téléphone de la mère :", es="Teléfono de la madre:")
E("student.label.father_name", old="Name (Father):", en="Father’s name:", fr="Nom du père :",
  es="Nombre del padre:")
E("student.label.father_job", old="Job (Father):", en="Father’s occupation:",
  fr="Profession du père :", es="Profesión del padre:")
E("student.label.father_phone", old="Phone (Father):", en="Father’s phone:",
  fr="Téléphone du père :", es="Teléfono del padre:")
E("student.mother_name", old="Name (Mother)", en="Mother’s name", fr="Nom de la mère",
  es="Nombre de la madre")
E("student.mother_job", old="Job (Mother)", en="Mother’s occupation", fr="Profession de la mère",
  es="Profesión de la madre")
E("student.mother_phone", old="Phone (Mother)", en="Mother’s phone", fr="Téléphone de la mère",
  es="Teléfono de la madre")
E("student.father_name", old="Name (Father)", en="Father’s name", fr="Nom du père",
  es="Nombre del padre")
E("student.father_job", old="Job (Father)", en="Father’s occupation", fr="Profession du père",
  es="Profesión del padre")
E("student.father_phone", old="Phone (Father)", en="Father’s phone", fr="Téléphone du père",
  es="Teléfono del padre")
E("student.label.lesson_since", old="Lesson since:", en="Lessons since:", fr="Cours depuis :",
  es="Clases desde:")
E("student.lesson_since", old="Lesson since", en="Lessons since", fr="Cours depuis", es="Clases desde")
E("student.label.total", old="Total:", en="Total:", fr="Total :", es="Total:",
  c="Followed by the total number of years of lessons.")
E("student.label.recital_interval", old="Personal Concert Interval:",
  en="Personal recital interval:", fr="Intervalle personnel entre auditions :",
  es="Intervalo personal entre audiciones:")
E("student.ensemble_expected", old="Participation in Ensemble Expected",
  en="Ensemble participation expected", fr="Participation à un ensemble attendue",
  es="Se espera participación en un conjunto")
E("student.label.inventory_number", old="Inventory Number:", en="Inventory number:",
  fr="Numéro d’inventaire :", es="Número de inventario:")
E("student.label.rented_since", old="Rented since:", en="Rented since:", fr="Loué depuis le :",
  es="Alquilado desde:")
E("student.rented_since", old="Rented since", en="Rented since", fr="Loué depuis le",
  es="Alquilado desde")
E("student.rental_description", old="Rental instrument description",
  en="Rental instrument description", fr="Désignation de l’instrument en location",
  es="Descripción del instrumento de alquiler")

# ---------------------------------------------------------------- archive
E("archive.entry", old="Archive Entry", en="Archive entry", fr="Fiche d’archive", es="Ficha archivada")
E("archive.delete_entry", old="Delete Archive Entry", en="Delete archive entry",
  fr="Supprimer la fiche d’archive", es="Eliminar ficha archivada",
  merged=["&Delete Archive Entry"])
E("archive.delete.confirm", old="Do you really want to delete the archive entry \"%1\" ?",
  en="Do you really want to delete the archive entry “%1”?",
  fr="Voulez-vous vraiment supprimer la fiche d’archive « %1 » ?",
  es="¿Seguro que desea eliminar la ficha archivada “%1”?", c="%1 = name of the entry")
E("archive.doc.notes_and_pieces", old="Lesson notes and music pieces", en="Lesson notes and pieces",
  fr="Notes de cours et morceaux", es="Notas de clase y obras")
E("archive.doc.lessons_taken", old="The student took part in the following lessons",
  en="The student attended the following lessons", fr="L’élève a suivi les cours suivants",
  es="El alumno ha asistido a las siguientes clases")
E("archive.doc.activities", old="Activities of the student", en="Student’s activities",
  fr="Activités de l’élève", es="Actividades del alumno")

# ---------------------------------------------------------------- notes
E("note.add", old="&Add new note", en="Add new note", fr="Ajouter une note", es="Añadir nota")
E("note.delete", old="&Delete note", en="Delete note", fr="Supprimer la note", es="Eliminar nota")
E("note.new", en="New note", de="Neue Notiz", fr="Nouvelle note", es="Nueva nota")
E("note.save", en="Save note", de="Notiz speichern", fr="Enregistrer la note", es="Guardar nota")
E("note.save_for_all", src="Save for all %n students",
  en=["Save for %n student", "Save for all %n students"],
  de=["Für %n Schüler speichern", "Für alle %n Schüler speichern"],
  fr=["Enregistrer pour %n élève", "Enregistrer pour les %n élèves"],
  es=["Guardar para %n alumno", "Guardar para los %n alumnos"],
  c="Switch in the note editor of group lessons.")
E("note.save_for_all_hint", en="The default can be changed in the settings",
  de="Voreinstellung in den Einstellungen", fr="Valeur par défaut modifiable dans les paramètres",
  es="El valor predeterminado se puede cambiar en los ajustes")
E("note.placeholder", en="Note for today’s lesson…", de="Notiz zur heutigen Stunde ...",
  fr="Note pour le cours d’aujourd’hui…", es="Nota para la clase de hoy…")
E("format.bold", en="Bold", de="Fett", fr="Gras", es="Negrita")
E("format.italic", en="Italic", de="Kursiv", fr="Italique", es="Cursiva")
E("format.underline", en="Underline", de="Unterstrichen", fr="Souligné", es="Subrayado")
E("format.text_color", en="Text color", de="Textfarbe", fr="Couleur du texte", es="Color del texto")

# ---------------------------------------------------------------- pieces
E("piece.add", old="&Add new Piece", en="Add new piece", fr="Ajouter un morceau", es="Añadir obra")
E("piece.delete", old="&Delete Piece", en="Delete piece", fr="Supprimer le morceau", es="Eliminar obra")
E("piece.required",
  old="You have to complete at least the fields \"Composer\" and \"Title\" to add the entry!",
  en="Please fill in at least “Composer” and “Title” to add the entry.",
  fr="Veuillez renseigner au moins « Compositeur » et « Titre » pour ajouter l’entrée.",
  es="Rellene al menos “Compositor” y “Título” para añadir la entrada.")
E("piece.status.planned", old="Planned", en="Planned", fr="Prévu", es="Planificada")
E("piece.status.in_progress", old="In Progress", en="In progress", fr="En cours", es="En curso")
E("piece.status.paused", old="Paused", en="Paused", fr="En pause", es="En pausa")
E("piece.status.ready", old="Ready for Concert", en="Ready to perform", fr="Prêt pour l’audition",
  es="Lista para audición")
E("piece.status.finished", old="Finished", en="Finished", fr="Terminé", es="Terminada")
E("piece.change_status", en="Change status", de="Status ändern", fr="Modifier le statut",
  es="Cambiar estado")
E("piece.add_to_event", en="Add to event", de="Zu Veranstaltung hinzufügen",
  fr="Ajouter à un événement", es="Añadir a un evento")
E("piece.delete_for_all", en="Delete (for all students)", de="Löschen (für alle Schüler)",
  fr="Supprimer (pour tous les élèves)", es="Eliminar (para todos los alumnos)")
E("piece.filter_status", en="Filter by status", de="Nach Status filtern", fr="Filtrer par statut",
  es="Filtrar por estado")

# ---------------------------------------------------------------- activities
E("activity.regular.heading", old="Regular (e.g. Ensembles):", en="Regular (e.g. ensembles):",
  fr="Régulières (p. ex. ensembles) :", es="Regulares (p. ej., conjuntos):")
E("activity.irregular.heading", old="Irregular (e.g. Concerts, Competitions):",
  en="One-off (e.g. recitals, competitions):", fr="Ponctuelles (p. ex. auditions, concours) :",
  es="Puntuales (p. ej., audiciones, concursos):")
E("activity.regular", old="Regular activities", en="Regular activities", fr="Activités régulières",
  es="Actividades regulares")
E("activity.irregular", old="Irregular activities", en="One-off activities", fr="Activités ponctuelles",
  es="Actividades puntuales")
E("activity.type.ensemble", old="Ensemble", en="Ensemble", fr="Ensemble", es="Conjunto")
E("activity.type.theory", old="Theory lesson", en="Music theory", fr="Formation musicale",
  es="Lenguaje musical")
E("activity.type.coaching", old="Coaching with pianists", en="Accompanist rehearsal",
  fr="Travail avec pianiste accompagnateur", es="Ensayo con pianista acompañante")
E("activity.type.solo", old="Solo Recital", en="Solo performance", fr="Prestation en solo",
  es="Actuación solista", merged=["(code) Solo-Vortrag"],
  note="1.4.1 zeigte in der Aktivitätenliste hartkodiert „Solo-Vortrag“, im Auswahlfeld „Solo-Beitrag“; übernommen ist „Solo-Beitrag“")
E("activity.type.ensemble_participation", old="Ensemble Recital", en="Ensemble participation",
  fr="Participation à un ensemble", es="Participación en un conjunto",
  merged=["(code) Ensemble-Mitwirkung"],
  note="1.4.1 zeigte in der Aktivitätenliste hartkodiert „Ensemble-Mitwirkung“; übernommen ist „Ensemblemitwirkung“")

# ---------------------------------------------------------------- instruments
E("instruments.title", old="Instrumenten Manager - Qupil", en="Instrument manager",
  fr="Gestionnaire d’instruments", es="Gestor de instrumentos")
E("instruments.tab.students", old="Pupils - Instruments", en="Students – instruments",
  fr="Élèves – instruments", es="Alumnos – instrumentos")
E("instruments.tab.instrument_count", old="Instruments - Number", en="Instruments – count",
  fr="Instruments – nombre", es="Instrumentos – cantidad")
E("instruments.tab.size_count", old="Sizes - Number", en="Sizes – count", fr="Tailles – nombre",
  es="Tamaños – cantidad")
E("instruments.statistics", old="Statistic", en="Statistics", fr="Statistiques", es="Estadísticas")
E("instruments.label.rental_count", old="Rental Instruments:", en="Rental instruments:",
  fr="Instruments en location :", es="Instrumentos de alquiler:")
E("instruments.label.next_size_count", old="Next size required:", en="Next size needed:",
  fr="Taille suivante nécessaire :", es="Siguiente tamaño necesario:")
E("instruments.next_size", old="Next size required", en="Next size needed",
  fr="Taille suivante nécessaire", es="Siguiente tamaño necesario", merged=["Next Size needed"])
E("instruments.inventory_list", old="Rental instrument inventory list", en="Rental instrument inventory",
  fr="Inventaire des instruments en location", es="Inventario de instrumentos de alquiler")
E("instruments.inventory_list_ellipsis", old="Rental Instruments Inventory List ...",
  en="Rental instrument inventory…", fr="Inventaire des instruments en location…",
  es="Inventario de instrumentos de alquiler…")
E("common.rental_instrument", old="Rental Instrument", en="Rental instrument",
  fr="Instrument en location", es="Instrumento de alquiler")

# ---------------------------------------------------------------- sheet music library
E("library.title", old="Music Library - Qupil", en="Sheet music library", fr="Bibliothèque de partitions",
  es="Biblioteca de partituras")
E("library.col.index", old="Index", en="No.", fr="N°", es="N.º", c="Inventory number column.")
E("library.col.author", old="Composer / Author", en="Composer / author", fr="Compositeur / auteur",
  es="Compositor / autor")
E("library.label.author", old="Composer / Author:", en="Composer / author:",
  fr="Compositeur / auteur :", es="Compositor / autor:")
E("library.col.publisher", old="Publisher", en="Publisher", fr="Éditeur", es="Editorial")
E("library.label.publisher", old="Publisher:", en="Publisher:", fr="Éditeur :", es="Editorial:")
E("library.tab.available", old="Available", en="Available", fr="Disponibles", es="Disponibles",
  c="Sheet music that is not lent out.")
E("library.tab.loaned", old="Loaned", en="On loan", fr="Prêtées", es="Prestadas")
E("library.col.lent_to", old="Lent to", en="Lent to", fr="Prêtée à", es="Prestada a")
E("library.col.lent_on", old="Lent on", en="Lent on", fr="Prêtée le", es="Prestada el",
  merged=["Loaned at"])
E("library.lend_to", old="Give selection to", en="Lend selection to", fr="Prêter la sélection à",
  es="Prestar la selección a")
E("library.returned", old="Selection returned by the pupil", en="Selection returned by student",
  fr="Sélection rendue par l’élève", es="Selección devuelta por el alumno",
  merged=["Selection &Returned by the Pupil"])
E("library.required",
  old="You must complete all fields (composer / author, title, publisher) \nto add the entry!",
  en="Please fill in all fields (composer / author, title, publisher) to add the entry.",
  fr="Veuillez renseigner tous les champs (compositeur / auteur, titre, éditeur) pour ajouter l’entrée.",
  es="Rellene todos los campos (compositor / autor, título, editorial) para añadir la entrada.")

# ---------------------------------------------------------------- events / recitals
E("event.manager.title", old="Event Manager - Qupil", en="Event manager",
  fr="Gestionnaire d’événements", es="Gestor de eventos", merged=["&Event Manager"])
E("event.tab.ready", old="&Ready for Concert", en="Pieces ready to perform",
  fr="Morceaux prêts pour l’audition", es="Obras listas para audición")
E("event.filter.location", old="Show pupils from:", en="Show students from:",
  fr="Afficher les élèves de :", es="Mostrar alumnos de:", c="Followed by a lesson location.")
E("event.filter.location_alt", old="Show students from this location:",
  en="Show students from this location:", fr="Afficher les élèves de ce lieu :",
  es="Mostrar alumnos de este lugar:")
E("event.col.student_age_instrument", old="Pupil (Age) - Instrument", en="Student (age) – instrument",
  fr="Élève (âge) – instrument", es="Alumno (edad) – instrumento")
E("event.col.musician_age_instrument", old="Musician (Age) - Instrument",
  en="Performer (age) – instrument", fr="Musicien (âge) – instrument",
  es="Intérprete (edad) – instrumento")
E("event.document", old="&Document ...", en="Document…", fr="Document…", es="Documento…")
E("event.archive_delete", old="Archive and Delete", en="Archive and delete", fr="Archiver et supprimer",
  es="Archivar y eliminar")
E("event.add_external", old="Add External Contribution ...", en="Add external contribution…",
  fr="Ajouter une prestation externe…", es="Añadir participación externa…")
E("event.delete.title", old="Delete Event", en="Delete event", fr="Supprimer l’événement",
  es="Eliminar evento")
E("event.delete.confirm", old="Do you really want to delete the event: \"%1\" ?",
  en="Do you really want to delete the event “%1”?",
  fr="Voulez-vous vraiment supprimer l’événement « %1 » ?",
  es="¿Seguro que desea eliminar el evento “%1”?", c="%1 = name of the event")
E("event.organizer", old="Organiser", en="Organizer", fr="Organisateur", es="Organizador")
E("event.label.organizer", old="Organiser:", en="Organizer:", fr="Organisateur :", es="Organizador:")
E("event.accompanist", old="Accompanist", en="Accompanist", fr="Accompagnateur", es="Acompañante")
E("event.label.accompanist", old="Accompanist:", en="Accompanist:", fr="Accompagnateur :",
  es="Acompañante:")
E("event.label.location", old="Location:", pick="Ort:", en="Location:", fr="Lieu :", es="Lugar:")
E("event.net_playing_time", old="Pure playing time", en="Net playing time",
  fr="Durée de jeu effective", es="Tiempo neto de interpretación")
E("event.estimated_total", old="Estimated total duration", en="Estimated total duration",
  fr="Durée totale estimée", es="Duración total estimada")
E("event.state.planned", old="planned", en="Planned", fr="Prévu", es="Planificado")
E("event.state.done", old="done", en="Done", fr="Terminé", es="Realizado")
E("event.add.title", old="Add Concert - Qupil", en="Add event", fr="Ajouter un événement",
  es="Añadir evento")
E("event.edit", en="Edit event", de="Veranstaltung bearbeiten", fr="Modifier l’événement",
  es="Editar evento")
E("event.external.title", old="Add External Concert Music - Qupil", en="Add external contribution",
  fr="Ajouter une prestation externe", es="Añadir participación externa")
E("event.external.musicians", old="Musicians:", en="Performers:", fr="Musiciens :", es="Intérpretes:")
E("event.external.format_hint", old="__HTML_FORMAT__",
  de="Format: Name, Vorname (Alter) - Instrument",
  en="Format: last name, first name (age) – instrument",
  fr="Format : nom, prénom (âge) – instrument",
  es="Formato: apellidos, nombre (edad) – instrumento",
  note="Aus dem HTML-Hinweistext des Designers extrahiert")
E("event.external.required",
  old="You need to fill at least the fields \"Composer\", \"Title\" und \"Musicians\"\nto add the entry!",
  en="Please fill in at least “Composer”, “Title” and “Performers” to add the entry.",
  fr="Veuillez renseigner au moins « Compositeur », « Titre » et « Musiciens » pour ajouter l’entrée.",
  es="Rellene al menos “Compositor”, “Título” e “Intérpretes” para añadir la entrada.")
E("event.external_badge", en="External", de="Extern", fr="Externe", es="Externa",
  c="Badge for a contribution by performers who are not your students.")
E("event.select_pieces.title", old="Concert Music Selection - Qupil", en="Select pieces",
  fr="Choisir des morceaux", es="Seleccionar obras")
E("event.select_pieces.hint", old="Please select one or more pieces to add to the current concert!",
  en="Select one or more pieces to add to this event.",
  fr="Sélectionnez un ou plusieurs morceaux à ajouter à cet événement.",
  es="Seleccione una o varias obras para añadirlas a este evento.")
E("event.finish.title", old="Automatic Functions - Qupil", en="Automatic actions",
  fr="Actions automatiques", es="Acciones automáticas")
E("event.finish.intro",
  old="Qupil can perform some automatic functions to assist you. Please select which functions should be executed:",
  en="Qupil can carry out a few actions automatically for you. Please choose which ones to run:",
  fr="Qupil peut effectuer automatiquement quelques actions pour vous. Choisissez celles à exécuter :",
  es="Qupil puede realizar algunas acciones automáticamente. Elija cuáles desea ejecutar:")
E("event.finish.activities",
  old="For each participant, the event is automatically entered as a one-time activity.  \nDo you want to use this feature?",
  en="The event is entered as a one-off activity for every participant.\nDo you want to use this feature?",
  fr="L’événement est ajouté comme activité ponctuelle pour chaque participant.\nVoulez-vous utiliser cette fonction ?",
  es="El evento se registra como actividad puntual para cada participante.\n¿Desea usar esta función?")
E("event.finish.pieces",
  old="The status of the performed music pieces is automatically set to \"completed\" for each participant.\nDo you want to use this feature?",
  en="The status of the performed pieces is set to “finished” for every participant.\nDo you want to use this feature?",
  fr="Le statut des morceaux joués passe à « terminé » pour chaque participant.\nVoulez-vous utiliser cette fonction ?",
  es="El estado de las obras interpretadas pasa a “terminada” para cada participante.\n¿Desea usar esta función?",
  note="Deutsch sagt „abgeschlossen“, der Werkstatus heißt aber „Beendet“ (Wortlaut belassen)")
E("event.program.doc_title", old="Program overview for events", en="Event program",
  fr="Programme de l’événement", es="Programa del evento")
E("event.program.heading", old="Program for", en="Program for “%1”", de="Programmübersicht für \"%1\"",
  fr="Programme de « %1 »", es="Programa de “%1”", c="%1 = name of the event",
  note="Fragment „Programmübersicht für“ mit Platzhalter für den Namen")
E("event.program.col_piece", old="Music Piece / Movements", en="Piece / movements",
  fr="Morceau / mouvements", es="Obra / movimientos")
E("event.reorder_hint", en="Drag the handle to reorder", de="Zum Sortieren am Griff ziehen",
  fr="Faites glisser la poignée pour réorganiser", es="Arrastre el tirador para cambiar el orden")
E("event.export_program", en="Program as PDF", de="Programm als PDF", fr="Programme en PDF",
  es="Programa en PDF")
E("event.duration_breakdown", en="Includes %1 min of presentation and %2 min per contribution",
  de="inkl. %1 Min. Moderation und %2 Min. je Beitrag",
  fr="Dont %1 min de présentation et %2 min par prestation",
  es="Incluye %1 min de presentación y %2 min por participación", c="%1, %2 = minutes")

# ---------------------------------------------------------------- candidates
E("candidates.recital.title", old="Concert Candidates - Qupil", en="Recital candidates",
  fr="Candidats aux auditions", es="Candidatos a audición")
E("candidates.recital.col_last", old="Last Concert Ago", en="Last recital", fr="Dernière audition",
  es="Última audición",
  c="Column header. The cells show e.g. '3 months ago' (candidates.ago.months); the German header already contains 'vor'.")
E("candidates.ago.months", old="%1 months", src="%n month(s) ago",
  en=["%n month ago", "%n months ago"], de=["%n Monat", "%n Monaten"],
  fr=["il y a %n mois", "il y a %n mois"], es=["hace %n mes", "hace %n meses"],
  c="Time since the last recital.", note="Plural ergänzt; „vor“ steckt im Spaltentitel")
E("candidates.ago.days", old="%1 days", src="%n day(s) ago",
  en=["%n day ago", "%n days ago"], de=["%n Tag", "%n Tagen"],
  fr=["il y a %n jour", "il y a %n jours"], es=["hace %n día", "hace %n días"],
  c="Time since the last recital.", note="Plural ergänzt")
E("candidates.recital.found", old="The following overdue concert candidates were identified:",
  en="The following overdue recital candidates were found:",
  fr="Les candidats aux auditions suivants sont en retard :",
  es="Se han encontrado los siguientes candidatos a audición pendientes:")
E("candidates.recital.none", old="There are currently no overdue concert candidates.<br><br>Good job!",
  en="There are currently no overdue recital candidates.\n\nGood job!",
  fr="Aucun candidat aux auditions en retard pour le moment.\n\nBeau travail !",
  es="Actualmente no hay candidatos a audición pendientes.\n\n¡Buen trabajo!")
E("candidates.recital.total", old="Total <b>%1</b> overdue Pupils", src="%n overdue student(s) in total",
  en=["%n overdue student in total", "%n overdue students in total"],
  de=["Insgesamt %n überfälliger Schüler", "Insgesamt %n überfällige Schüler"],
  fr=["%n élève en retard au total", "%n élèves en retard au total"],
  es=["%n alumno pendiente en total", "%n alumnos pendientes en total"],
  note="HTML entfernt, Plural ergänzt")
E("candidates.recital.add_reminder", old="Set up reminder for concert activity",
  en="Set up reminder about recital activity", fr="Créer un rappel concernant les auditions",
  es="Crear recordatorio sobre audiciones")
E("candidates.recital.reminder_text", old="Speak about concert activity!",
  en="Talk about performing in a recital!", fr="Parler de la participation aux auditions !",
  es="¡Hablar de participar en una audición!", c="Default text of the created reminder.")
E("candidates.recital.reminder_created",
  old="Reminder concerning \"concert activity\"<br>set up for the pupil <i><b>%1</b></i>",
  en="Reminder about “recital activity” set up for %1.",
  fr="Rappel « auditions » créé pour %1.",
  es="Recordatorio sobre “audiciones” creado para %1.", c="%1 = student name")
E("candidates.ensemble.title", old="Ensemble Candidates - Qupil", en="Ensemble candidates",
  fr="Candidats aux ensembles", es="Candidatos a conjunto")
E("candidates.ensemble.col_last", old="Last Ensemble Participation", en="Last ensemble participation",
  fr="Dernière participation à un ensemble", es="Última participación en un conjunto")
E("candidates.ensemble.found", old="The following students are currently not active in any ensemble:",
  en="The following students are currently not active in any ensemble:",
  fr="Les élèves suivants ne participent actuellement à aucun ensemble :",
  es="Los siguientes alumnos no participan actualmente en ningún conjunto:")
E("candidates.ensemble.none", old="Currently all students are active in ensembles.<br><br>Good job!",
  en="All students are currently active in ensembles.\n\nGood job!",
  fr="Tous les élèves participent actuellement à un ensemble.\n\nBeau travail !",
  es="Actualmente todos los alumnos participan en conjuntos.\n\n¡Buen trabajo!")
E("candidates.ensemble.total",
  old="Total <span style='color:red; font-weight:600;'>%1</span> inactive pupils | <span style='color:green; font-weight:600;'>%2</span> pupils are active in ensembles",
  en="Total: %1 inactive students | %2 students active in ensembles",
  fr="Au total : %1 élèves inactifs | %2 élèves actifs dans un ensemble",
  es="Total: %1 alumnos inactivos | %2 alumnos activos en conjuntos",
  c="%1, %2 = numbers. The UI highlights them itself.", note="HTML-Farben entfernt")
E("candidates.ensemble.add_reminder", old="Set up reminder for ensemble activity",
  en="Set up reminder about ensemble activity", fr="Créer un rappel concernant les ensembles",
  es="Crear recordatorio sobre conjuntos")
E("candidates.ensemble.reminder_text", old="Speak about ensemble activity!",
  en="Talk about joining an ensemble!", fr="Parler de la participation à un ensemble !",
  es="¡Hablar de participar en un conjunto!", c="Default text of the created reminder.")
E("candidates.ensemble.reminder_created",
  old="Reminder concerning \"ensemble activity\"<br>set up for the pupil <i><b>%1</b></i>",
  en="Reminder about “ensemble activity” set up for %1.",
  fr="Rappel « ensembles » créé pour %1.",
  es="Recordatorio sobre “conjuntos” creado para %1.", c="%1 = student name")

# ---------------------------------------------------------------- reminders
E("reminders.title", old="Reminder - Qupil", en="Reminders", fr="Rappels", es="Recordatorios")
E("reminders.col.text", old="Reminder", en="Reminder", fr="Rappel", es="Recordatorio")
E("reminders.col.mode", old="Mode", en="When", fr="Quand", es="Cuándo")
E("reminders.col.sound", old="Sound", en="Sound", fr="Son", es="Sonido")
E("reminder.mode.startup", old="At program start", en="When Qupil starts", fr="Au démarrage de Qupil",
  es="Al iniciar Qupil")
E("reminder.mode.every_lesson", old="At every lesson", en="At the start of every lesson",
  fr="Au début de chaque cours", es="Al comienzo de cada clase")
E("reminder.mode.student", old="At the pupil", en="For student “%1”", de="Bei dem Schüler: \"%1\"",
  fr="Pour l’élève « %1 »", es="Para el alumno “%1”", c="%1 = student name",
  note="Fragment „Bei dem Schüler“ mit Platzhalter (so wurde es angezeigt)")
E("reminder.edit.title", old="Edit Reminder - Qupil", en="Edit reminder", fr="Modifier le rappel",
  es="Editar recordatorio")
E("reminder.label.text", old="Reminder Text:", en="Reminder text:", fr="Texte du rappel :",
  es="Texto del recordatorio:")
E("reminder.label.mode", old="Reminder Mode", en="When to remind", fr="Quand rappeler",
  es="Cuándo recordar")
E("reminder.mode.startup_long", old="Remember at programm start", en="Remind me when Qupil starts",
  fr="Me le rappeler au démarrage de Qupil", es="Recordármelo al iniciar Qupil")
E("reminder.mode.every_lesson_long", old="Remember at the beginning of each lesson",
  en="Remind me at the start of each lesson", fr="Me le rappeler au début de chaque cours",
  es="Recordármelo al comienzo de cada clase")
E("reminder.mode.student_long", old="Remember only with the following student:",
  en="Remind me only for this student:", fr="Me le rappeler uniquement pour cet élève :",
  es="Recordármelo solo para este alumno:")
E("reminder.play_sound", old="Play sound file while displaying the reminder",
  en="Play a sound when the reminder appears", fr="Jouer un son à l’affichage du rappel",
  es="Reproducir un sonido al mostrar el recordatorio")
E("reminder.text_required", old="Please enter a reminder text to use the function!",
  en="Please enter a reminder text.", fr="Veuillez saisir le texte du rappel.",
  es="Introduzca el texto del recordatorio.")
E("reminder.error.list", old="The following error occurred while reading the reminder list",
  en="The following error occurred while loading the reminders",
  fr="L’erreur suivante s’est produite lors du chargement des rappels",
  es="Se produjo el siguiente error al cargar los recordatorios",
  c="The error text follows on the next line.")
E("reminder.error.load", old="The following error occurred while loading the data",
  en="The following error occurred while loading the data",
  fr="L’erreur suivante s’est produite lors du chargement des données",
  es="Se produjo el siguiente error al cargar los datos", c="The error text follows on the next line.")
E("reminder.error.delete", old="The following error occurred while deleting the reminder",
  en="The following error occurred while deleting the reminder",
  fr="L’erreur suivante s’est produite lors de la suppression du rappel",
  es="Se produjo el siguiente error al eliminar el recordatorio",
  c="The error text follows on the next line.")
E("reminder.error.save", old="The following error occurred while entering the reminder",
  en="The following error occurred while saving the reminder",
  fr="L’erreur suivante s’est produite lors de l’enregistrement du rappel",
  es="Se produjo el siguiente error al guardar el recordatorio",
  c="The error text follows on the next line.")
E("reminder.heading", code="Erinnerung:", de="Erinnerung:", en="Reminder:", fr="Rappel :",
  es="Recordatorio:", note="In 1.4.1 hartkodiert (mainwindowimpl.cpp)")
E("reminder.for_student", old="Reminder for the pupil ", en="Reminder for student “%1”:",
  de="Erinnerung für den Schüler \"%1\":", fr="Rappel pour l’élève « %1 » :",
  es="Recordatorio para el alumno “%1”:", c="%1 = student name",
  note="Fragment mit Platzhalter (so wurde es angezeigt)")
E("reminder.done_remove", old="Done? - Reminder will be removed!",
  en="Done? The reminder will be removed.", fr="Terminé ? Le rappel sera supprimé.",
  es="¿Hecho? El recordatorio se eliminará.", c="Checkbox in the reminder popup.")
E("reminder.created", old="Reminder set up", en="Reminder set up", fr="Rappel créé",
  es="Recordatorio creado")

# ---------------------------------------------------------------- birthdays
E("birthday.title", old="Birthdays - Qupil", en="Birthdays", fr="Anniversaires", es="Cumpleaños")
E("birthday.today_one", old="The following student has his birthday today:",
  en="This student has a birthday today:", fr="Cet élève fête son anniversaire aujourd’hui :",
  es="Este alumno cumple años hoy:")
E("birthday.today_many", old="The following students have their birthday today:",
  en="These students have a birthday today:", fr="Ces élèves fêtent leur anniversaire aujourd’hui :",
  es="Estos alumnos cumplen años hoy:")
E("birthday.last_week_one", old="The following student had his birthday last week:",
  en="This student had a birthday last week:",
  fr="Cet élève a fêté son anniversaire la semaine dernière :",
  es="Este alumno cumplió años la semana pasada:")
E("birthday.last_week_many", old="The following students had their birthday last week:",
  en="These students had a birthday last week:",
  fr="Ces élèves ont fêté leur anniversaire la semaine dernière :",
  es="Estos alumnos cumplieron años la semana pasada:")
E("birthday.none", old="In the last week no student had his birthday!",
  en="No student had a birthday in the last week.",
  fr="Aucun élève n’a fêté son anniversaire la semaine dernière.",
  es="Ningún alumno cumplió años la semana pasada.")

# ---------------------------------------------------------------- metronome / tuning
E("metronome.title", old="Metronome", en="Metronome", fr="Métronome", es="Metrónomo")
E("metronome.window_title", old="Metronome / Tuner - Qupil", en="Metronome / tuning",
  fr="Métronome / accordage", es="Metrónomo / afinación")
E("metronome.label.tempo", old="Tempo:", en="Tempo:", fr="Tempo :", es="Tempo:")
E("metronome.unit.bpm", old=" bpm", en="bpm", fr="BPM", es="ppm", c="Beats per minute, after a number.")
E("metronome.accent_every", old="Accent every", en="Accent every", fr="Accent tous les",
  es="Acento cada", c="Sentence with a number field: [Accent every] [4] [beats].")
E("metronome.beats", old="Beats", en="beats", fr="temps", es="tiempos",
  c="Sentence with a number field: [Accent every] [4] [beats].")
E("metronome.tap_hint", old="Click rhythmically on the button to set the tempo.",
  en="Tap the button in time to set the tempo.", fr="Tapez sur le bouton en rythme pour régler le tempo.",
  es="Pulse el botón al ritmo deseado para ajustar el tempo.")
E("metronome.tab.tuning", old="Tune", pick="Stimmen", en="Tuning", fr="Accordage", es="Afinación")
E("metronome.label.tuning_note", old="Tuning tone:", en="Tuning note:", fr="Note d’accord :",
  es="Nota de afinación:")
E("metronome.label.pitch", old="Tuning pitch:", en="Concert pitch:", fr="Diapason :", es="Diapasón:",
  c="Frequency of A, e.g. 442 Hz.")
E("metronome.note.a", old="A", en="A", fr="La", es="La", c="Note name of the tuning note A.")
E("metronome.note.b", en="B", de="H", fr="Si", es="Si",
  c="Note name of B natural (494 Hz at A = 440 Hz). German calls this note 'H'.")
E("metronome.note.b_flat", old="B", de="B", en="B♭", fr="Si♭", es="Si♭",
  c="Note name of B-flat (466 Hz at A = 440 Hz). German calls this note 'B'.",
  note="KORREKTUR: 1.4.1 zeigte „H“, die Tondatei klingt aber mit 466 Hz = B")
E("metronome.load_failed", old="Could not load %1", en="Could not load %1.",
  fr="Impossible de charger %1.", es="No se pudo cargar %1.", c="%1 = file name")
E("audio.init_failed", code="Konnte die Soundkarte nicht initialisieren.",
  de="Konnte die Soundkarte nicht initialisieren.", en="Could not initialize audio output.",
  fr="Impossible d’initialiser la sortie audio.", es="No se pudo inicializar la salida de audio.",
  note="In 1.4.1 hartkodiert (sdlplayer.cpp)")
E("metronome.tap", en="Tap tempo", de="Im Takt tippen", fr="Taper le tempo", es="Marcar el tempo")
E("metronome.start", en="Start", de="Start", fr="Démarrer", es="Iniciar")
E("metronome.stop", en="Stop", de="Stopp", fr="Arrêter", es="Detener")
E("metronome.slower", en="Slower", de="Langsamer", fr="Plus lent", es="Más lento")
E("metronome.faster", en="Faster", de="Schneller", fr="Plus rapide", es="Más rápido")
E("metronome.bpm_long", en="Beats per minute", de="Schläge pro Minute", fr="Battements par minute",
  es="Pulsaciones por minuto")
E("metronome.keeps_running", en="Keeps running when you switch screens.",
  de="Läuft weiter, wenn Sie die Ansicht wechseln.",
  fr="Continue de jouer quand vous changez d’écran.", es="Sigue sonando al cambiar de pantalla.")
E("metronome.play_tone", en="Play tone", de="Ton spielen", fr="Jouer la note", es="Tocar la nota")

# ---------------------------------------------------------------- documents
E("doc.viewer.title", old="Document Viewer - Qupil", en="Document preview", fr="Aperçu du document",
  es="Vista previa del documento")
E("doc.dayview.create_title", old="Create Daily Schedule - Qupil", en="Create daily overview",
  fr="Créer l’aperçu du jour", es="Crear resumen del día")
E("doc.dayview.title", old="Daily schedule", en="Daily overview", fr="Aperçu du jour",
  es="Resumen del día")
E("doc.dayview.show_last", old="Show the last", en="Show the last", fr="Afficher les",
  es="Mostrar las últimas",
  c="Sentence with a number field: [Show the last] [3] [notes for each lesson].")
E("doc.dayview.notes_per_lesson", old="notes for every lesson", en="notes for each lesson",
  fr="dernières notes de chaque cours", es="notas de cada clase",
  c="Sentence with a number field: [Show the last] [3] [notes for each lesson].")
E("doc.dayview.add_space", old="Add around", en="Add about", fr="Ajouter environ", es="Añadir unos",
  c="Sentence with a number field: [Add about] [5 cm] [of free space for handwritten notes].")
E("doc.dayview.space_for_notes", old="free space for handwritten notes",
  en="of free space for handwritten notes", fr="d’espace libre pour des notes manuscrites",
  es="de espacio libre para notas a mano",
  c="Sentence with a number field: [Add about] [5 cm] [of free space for handwritten notes].")
E("doc.share_pdf", en="Share PDF", de="PDF teilen", fr="Partager le PDF", es="Compartir PDF")

# ---------------------------------------------------------------- CSV import
E("import.csv.title", old="CSV Address Book Import - Qupil", en="Address book import (CSV)",
  fr="Import de carnet d’adresses (CSV)", es="Importación de libreta de direcciones (CSV)",
  merged=["CSV Address Book Import"])
E("import.csv.encoding", old="File Coding", en="File encoding", fr="Encodage du fichier",
  es="Codificación del archivo")
E("import.csv.assign",
  old="Please assign the database fields to the corresponding fields from your CSV-File:",
  en="Please assign the matching column of your CSV file to each database field:",
  fr="Veuillez associer à chaque champ de la base de données la colonne correspondante de votre fichier CSV :",
  es="Asigne a cada campo de la base de datos la columna correspondiente de su archivo CSV:")
E("import.csv.skip_field", old="!!!Do Not Import This Field!!!", en="– Do not import this field –",
  fr="– Ne pas importer ce champ –", es="– No importar este campo –")
E("import.csv.no_separator",
  old="Separator of the CSV file (usually comma or semicolon) could not be recognized.\nCSV import is aborted!",
  en="The separator of the CSV file (usually a comma or semicolon) could not be detected.\nThe CSV import has been canceled.",
  fr="Le séparateur du fichier CSV (généralement une virgule ou un point-virgule) n’a pas pu être détecté.\nL’import CSV est annulé.",
  es="No se pudo detectar el separador del archivo CSV (normalmente una coma o un punto y coma).\nSe ha cancelado la importación CSV.")
E("import.csv.open_failed", old="CSV file could not be opened.\nCSV import is aborted!",
  en="The CSV file could not be opened.\nThe CSV import has been canceled.",
  fr="Le fichier CSV n’a pas pu être ouvert.\nL’import CSV est annulé.",
  es="No se pudo abrir el archivo CSV.\nSe ha cancelado la importación CSV.")
E("import.csv.done",
  old="Importing the data from the CSV file was completed successfully! \n %1 entries added",
  src="The CSV import completed successfully.\n%n entry(ies) added.",
  en=["The CSV import completed successfully.\n%n entry added.",
      "The CSV import completed successfully.\n%n entries added."],
  de=["Das Importieren der Daten aus der CSV-Datei wurde erfolgreich beendet!\nEs wurde %n Eintrag hinzugefügt",
      "Das Importieren der Daten aus der CSV-Datei wurde erfolgreich beendet!\nEs wurden %n Einträge hinzugefügt"],
  fr=["L’import CSV s’est terminé avec succès.\n%n entrée ajoutée.",
      "L’import CSV s’est terminé avec succès.\n%n entrées ajoutées."],
  es=["La importación CSV ha finalizado correctamente.\nSe ha añadido %n entrada.",
      "La importación CSV ha finalizado correctamente.\nSe han añadido %n entradas."],
  note="Plural ergänzt, Tippfehler „Einräge“ → „Einträge“")
E("import.open_file", old="Open Address File - Qupil", en="Open address file",
  fr="Ouvrir un fichier d’adresses", es="Abrir archivo de direcciones")
E("import.filter_csv", old="Address CSV (*.csv)", en="Address CSV (*.csv)", fr="CSV d’adresses (*.csv)",
  es="CSV de direcciones (*.csv)", c="File dialog filter. Keep the pattern in parentheses unchanged.")

# ---------------------------------------------------------------- backup / database
E("backup.select_dir", old="Select Backup Directory", en="Select backup folder",
  fr="Choisir le dossier de sauvegarde", es="Seleccionar carpeta de copias")
E("backup.create.title", old="Create Backup - Qupil", en="Create backup", fr="Créer une sauvegarde",
  es="Crear copia de seguridad")
E("backup.create.success", old="The backup with the file name \"%1\" was successfully created!",
  en="The backup “%1” was created successfully.", fr="La sauvegarde « %1 » a été créée.",
  es="La copia de seguridad “%1” se ha creado correctamente.", c="%1 = file name")
E("backup.create.failed",
  old="The backup could not be created in the selected directory \"%1\". \nPlease check if you have write permission for this directory!",
  en="The backup could not be created in the folder “%1”.\nPlease check that you have write permission for this folder.",
  fr="La sauvegarde n’a pas pu être créée dans le dossier « %1 ».\nVérifiez que vous avez les droits d’écriture sur ce dossier.",
  es="No se pudo crear la copia de seguridad en la carpeta “%1”.\nCompruebe que tiene permiso de escritura en esa carpeta.",
  c="%1 = folder")
E("backup.open.title", old="Open Backup - Qupil", en="Open backup", fr="Ouvrir une sauvegarde",
  es="Abrir copia de seguridad")
E("backup.filter", old="Qupil Backup File (*.db)", en="Qupil backup file (*.db)",
  fr="Fichier de sauvegarde Qupil (*.db)", es="Archivo de copia de seguridad de Qupil (*.db)",
  c="File dialog filter. Keep the pattern in parentheses unchanged.")
E("backup.restore.title", old="Replay Backup - Qupil", en="Restore backup", fr="Restaurer une sauvegarde",
  es="Restaurar copia de seguridad")
E("backup.restore.warning",
  old="A backup should only be restored as data recovery after data loss. \nIf you restore a backup, changes and new entries \nthat may have been written after the backup will be deleted. \n\nWould you really like to restore the backup?",
  en="A backup should only be restored to recover from data loss.\nIf you restore a backup, any changes and new entries made after the backup will be lost.\n\nDo you really want to restore the backup?",
  fr="Une sauvegarde ne doit être restaurée qu’en cas de perte de données.\nSi vous restaurez une sauvegarde, les modifications et nouvelles entrées effectuées après la sauvegarde seront perdues.\n\nVoulez-vous vraiment restaurer la sauvegarde ?",
  es="Una copia de seguridad solo debe restaurarse para recuperar datos perdidos.\nSi restaura una copia, se perderán los cambios y las entradas nuevas posteriores a ella.\n\n¿Seguro que desea restaurar la copia de seguridad?")
E("backup.restore.success", old="The backup has been successfully restored. \nPlease restart Qupil!",
  en="The backup was restored successfully.\nPlease restart Qupil.",
  fr="La sauvegarde a été restaurée.\nVeuillez redémarrer Qupil.",
  es="La copia de seguridad se ha restaurado correctamente.\nReinicie Qupil.")
E("backup.restore.unknown_type",
  old="The backup type could not be determined! Therefore, the backup can not be restored. \nPlease contact the program author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> to fix this problem!",
  en="The backup type could not be determined, so the backup cannot be restored.\nPlease contact the author at %1 to fix this problem.",
  fr="Le type de sauvegarde n’a pas pu être déterminé ; la sauvegarde ne peut donc pas être restaurée.\nVeuillez contacter l’auteur à l’adresse %1 pour résoudre ce problème.",
  es="No se pudo determinar el tipo de copia, por lo que no se puede restaurar.\nPóngase en contacto con el autor en %1 para resolver el problema.",
  email=True, c="%1 = contact e-mail address", note="E-Mail-Adresse als Platzhalter, Link-HTML entfernt")
E("backup.restore.failed",
  old="The backup could not be restored! \nPlease contact the program author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> to fix this issue!",
  en="The backup could not be restored.\nPlease contact the author at %1 to fix this problem.",
  fr="La sauvegarde n’a pas pu être restaurée.\nVeuillez contacter l’auteur à l’adresse %1 pour résoudre ce problème.",
  es="No se pudo restaurar la copia de seguridad.\nPóngase en contacto con el autor en %1 para resolver el problema.",
  email=True, c="%1 = contact e-mail address", note="E-Mail-Adresse als Platzhalter, Link-HTML entfernt")
E("backup.restore.empty",
  old="The selected backup file does not exist or is empty. \nThe backup will not be restored!",
  en="The selected backup file does not exist or is empty.\nThe backup will not be restored.",
  fr="Le fichier de sauvegarde sélectionné n’existe pas ou est vide.\nLa sauvegarde ne sera pas restaurée.",
  es="El archivo de copia seleccionado no existe o está vacío.\nNo se restaurará la copia de seguridad.")
E("db.update_failed",
  old="An error occurred while updating the database structure. Qupil may not be able to process the data correctly. \nPlease contact the program author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> to fix this issue!",
  en="An error occurred while updating the database structure. Qupil may not be able to process the data correctly.\nPlease contact the author at %1 to fix this problem.",
  fr="Une erreur s’est produite lors de la mise à jour de la structure de la base de données. Qupil risque de ne pas traiter les données correctement.\nVeuillez contacter l’auteur à l’adresse %1 pour résoudre ce problème.",
  es="Se produjo un error al actualizar la estructura de la base de datos. Es posible que Qupil no procese los datos correctamente.\nPóngase en contacto con el autor en %1 para resolver el problema.",
  email=True, c="%1 = contact e-mail address", note="E-Mail-Adresse als Platzhalter, Link-HTML entfernt")
E("db.update_failed_detail",
  old="When updating the database structure from revision %0 to %1, the following error occurred:<br><b>%2</b><br>Please contact the program author: <a href=\"mailto:fhammer@qupil.de\">fhammer@qupil.de</a> to fix this problem!",
  en="While updating the database structure from revision %1 to %2, the following error occurred:\n%3\nPlease contact the author at %4 to fix this problem.",
  de="Beim Update der Datenbankstruktur von Revision %1 auf %2 ist folgender Fehler aufgetreten:\n%3\nBitte wenden Sie sich an den Programm-Autor: %4 um dieses Problem zu beheben!",
  fr="L’erreur suivante s’est produite lors de la mise à jour de la structure de la base de données de la révision %1 à %2 :\n%3\nVeuillez contacter l’auteur à l’adresse %4 pour résoudre ce problème.",
  es="Se produjo el siguiente error al actualizar la estructura de la base de datos de la revisión %1 a la %2:\n%3\nPóngase en contacto con el autor en %4 para resolver el problema.",
  c="%1, %2 = revision numbers, %3 = error text, %4 = contact e-mail address",
  note="Platzhalter %0–%2 → %1–%3 (Qt kennt kein %0), E-Mail als %4, HTML entfernt, „Author“ → „Autor“")
E("db.newer_version",
  en="This data was created with a newer version of Qupil (data version %1). Please update Qupil.",
  de="Diese Daten wurden mit einer neueren Qupil-Version erstellt (Datenversion %1). Bitte aktualisieren Sie Qupil.",
  fr="Ces données ont été créées avec une version plus récente de Qupil (version des données %1). Veuillez mettre Qupil à jour.",
  es="Estos datos se crearon con una versión más reciente de Qupil (versión de datos %1). Actualice Qupil.",
  c="%1 = data version number")
E("migration.importing", en="Importing data from Qupil 1.x…", de="Übernehme Daten aus Qupil 1.x ...",
  fr="Importation des données de Qupil 1.x…", es="Importando datos de Qupil 1.x…")
E("migration.done", en="Your data from Qupil 1.x has been imported.",
  de="Ihre Daten aus Qupil 1.x wurden übernommen.", fr="Vos données de Qupil 1.x ont été importées.",
  es="Se han importado sus datos de Qupil 1.x.")
E("migration.pre10",
  en="Data from Qupil versions before 1.0 cannot be imported directly. Please open it once with Qupil 1.4.1 first.",
  de="Daten aus Qupil-Versionen vor 1.0 können nicht direkt übernommen werden. Bitte öffnen Sie diese zuerst einmal mit Qupil 1.4.1.",
  fr="Les données des versions de Qupil antérieures à 1.0 ne peuvent pas être importées directement. Ouvrez-les d’abord une fois avec Qupil 1.4.1.",
  es="Los datos de versiones de Qupil anteriores a la 1.0 no se pueden importar directamente. Ábralos primero una vez con Qupil 1.4.1.")
E("backup.auto", en="Automatic backup", de="Automatisches Backup", fr="Sauvegarde automatique",
  es="Copia de seguridad automática")
E("backup.keep", src="Keep the last %n backup(s)",
  en=["Keep the last backup", "Keep the last %n backups"],
  de=["Nur das letzte Backup behalten", "Die letzten %n Backups behalten"],
  fr=["Conserver la dernière sauvegarde", "Conserver les %n dernières sauvegardes"],
  es=["Conservar la última copia", "Conservar las últimas %n copias"])
E("backup.password", en="Backup password (optional)", de="Backup-Passwort (optional)",
  fr="Mot de passe de la sauvegarde (facultatif)", es="Contraseña de la copia (opcional)")
E("backup.last", en="Last backup: %1", de="Letztes Backup: %1", fr="Dernière sauvegarde : %1",
  es="Última copia: %1", c="%1 = date and time")
E("backup.restore.device", en="Restore this device", de="Gerät wiederherstellen",
  fr="Restaurer cet appareil", es="Restaurar este dispositivo")
E("backup.restore.device_hint",
  en="This device gets the state of the backup and then fetches all changes from your other devices.",
  de="Dieses Gerät erhält den Stand des Backups und holt sich anschließend alle Änderungen Ihrer anderen Geräte.",
  fr="Cet appareil reprend l’état de la sauvegarde, puis récupère toutes les modifications de vos autres appareils.",
  es="Este dispositivo recupera el estado de la copia y después obtiene todos los cambios de sus otros dispositivos.")
E("backup.restore.all", en="Restore for all devices", de="Stand für alle Geräte übernehmen",
  fr="Restaurer pour tous les appareils", es="Restaurar en todos los dispositivos")
E("backup.restore.all_hint", en="Changes made after the backup are undone on all devices.",
  de="Änderungen nach dem Backup werden auf allen Geräten rückgängig gemacht.",
  fr="Les modifications postérieures à la sauvegarde sont annulées sur tous les appareils.",
  es="Los cambios posteriores a la copia se deshacen en todos los dispositivos.")

# ---------------------------------------------------------------- first start, take-over, data (new)
E("setup.welcome", en="Welcome to Qupil 2", de="Willkommen bei Qupil 2", fr="Bienvenue dans Qupil 2",
  es="Bienvenido a Qupil 2")
E("setup.question", en="How would you like to start?", de="Wie möchten Sie beginnen?",
  fr="Comment souhaitez-vous commencer ?", es="¿Cómo desea empezar?")
E("setup.legacy.title", en="Take over data from Qupil 1.x", de="Daten aus Qupil 1.x übernehmen",
  fr="Reprendre les données de Qupil 1.x", es="Importar los datos de Qupil 1.x")
E("setup.legacy.text",
  en="Students, lessons, notes, pieces, events and the sheet music library are taken over. Your Qupil 1.x data stays unchanged.",
  de="Schüler, Unterrichte, Notizen, Werke, Veranstaltungen und die Noten-Bibliothek werden übernommen. Ihre Daten in Qupil 1.x bleiben dabei unverändert.",
  fr="Les élèves, cours, notes, morceaux, événements et la bibliothèque de partitions sont repris. Vos données de Qupil 1.x restent inchangées.",
  es="Se importan alumnos, clases, notas, obras, eventos y la biblioteca de partituras. Sus datos de Qupil 1.x no se modifican.")
E("setup.legacy.found", en="Found: %1", de="Gefunden: %1", fr="Trouvé : %1", es="Encontrado: %1",
  c="%1 = folder with Qupil 1.x data, e.g. ~/.qupil")
E("setup.legacy.action", en="Take over", de="Übernehmen", fr="Reprendre", es="Importar",
  c="Button: import the data of Qupil 1.x")
E("setup.legacy.choose_folder", en="Choose folder…", de="Ordner wählen ...", fr="Choisir un dossier…",
  es="Elegir carpeta…")
E("setup.legacy.choose_file", en="Choose file…", de="Datei wählen ...", fr="Choisir un fichier…",
  es="Elegir archivo…")
E("setup.legacy.where", en="Qupil 1.x keeps its data in the folder %1.",
  de="Qupil 1.x speichert seine Daten im Ordner %1.", fr="Qupil 1.x enregistre ses données dans le dossier %1.",
  es="Qupil 1.x guarda sus datos en la carpeta %1.", c="%1 = folder, e.g. ~/.qupil or %APPDATA%\\qupil")
E("setup.legacy.mobile_hint",
  en="First copy the file “qupil.db” from the “db” folder of your Qupil 1.x data to this device, e.g. via cloud storage or the Files app.",
  de="Kopieren Sie zuerst die Datei „qupil.db“ aus dem Ordner „db“ Ihrer Qupil-1.x-Daten auf dieses Gerät, z. B. über einen Cloud-Speicher oder die Dateien-App.",
  fr="Copiez d’abord le fichier « qupil.db » du dossier « db » de vos données Qupil 1.x sur cet appareil, par exemple via un stockage en ligne ou l’app Fichiers.",
  es="Copie primero el archivo “qupil.db” de la carpeta “db” de sus datos de Qupil 1.x a este dispositivo, por ejemplo mediante un almacenamiento en la nube o la app Archivos.")
E("setup.legacy.hint_title", en="About taking over your data", de="Hinweis zur Datenübernahme",
  fr="À propos de la reprise des données", es="Acerca de la importación de datos")
E("setup.legacy.hint",
  en="Qupil 2 keeps its data in a database of its own. Your Qupil 1.x data is only read, never changed. Changes you make in Qupil 1.x afterwards are not taken over automatically – you can repeat the take-over at any time under More › Data.",
  de="Qupil 2 speichert seine Daten in einer eigenen Datenbank. Ihre Qupil-1.x-Daten werden dabei nur gelesen und nicht verändert. Änderungen, die Sie danach noch in Qupil 1.x vornehmen, werden nicht automatisch übernommen – Sie können die Übernahme jederzeit unter Mehr › Daten wiederholen.",
  fr="Qupil 2 enregistre ses données dans sa propre base de données. Vos données Qupil 1.x sont seulement lues, jamais modifiées. Les modifications faites ensuite dans Qupil 1.x ne sont pas reprises automatiquement – vous pouvez relancer la reprise à tout moment dans Plus › Données.",
  es="Qupil 2 guarda sus datos en una base de datos propia. Sus datos de Qupil 1.x solo se leen, nunca se modifican. Los cambios que haga después en Qupil 1.x no se importan automáticamente; puede repetir la importación en cualquier momento en Más › Datos.")
E("setup.backup.text", en="A backup file (.db) from Qupil 1.x or Qupil 2.",
  de="Eine Sicherungsdatei (.db) aus Qupil 1.x oder Qupil 2.",
  fr="Un fichier de sauvegarde (.db) de Qupil 1.x ou de Qupil 2.",
  es="Un archivo de copia de seguridad (.db) de Qupil 1.x o de Qupil 2.")
E("setup.backup.action", en="Choose backup…", de="Backup wählen ...", fr="Choisir une sauvegarde…",
  es="Elegir copia de seguridad…")
E("setup.fresh.title", en="Start from scratch", de="Neu beginnen", fr="Commencer à zéro",
  es="Empezar desde cero")
E("setup.fresh.text", en="Start with empty data. You can take over data later at any time under More › Data.",
  de="Mit leeren Daten starten. Eine Übernahme ist später jederzeit unter Mehr › Daten möglich.",
  fr="Commencer avec des données vides. Vous pourrez reprendre des données plus tard dans Plus › Données.",
  es="Empezar sin datos. Podrá importar datos más adelante en Más › Datos.")
E("setup.fresh.action", en="Start", de="Starten", fr="Commencer", es="Empezar")
E("setup.continue", en="Continue", de="Weiter", fr="Continuer", es="Continuar")
E("setup.error.open", en="Your data could not be opened: %1", de="Ihre Daten konnten nicht geöffnet werden: %1",
  fr="Vos données n’ont pas pu être ouvertes : %1", es="No se han podido abrir sus datos: %1",
  c="%1 = technical error message")
E("setup.error.kept",
  en="The existing file is kept in the backup folder when you restore a backup or start from scratch.",
  de="Die vorhandene Datei bleibt im Backup-Ordner erhalten, wenn Sie ein Backup zurückspielen oder neu beginnen.",
  fr="Le fichier existant est conservé dans le dossier des sauvegardes si vous restaurez une sauvegarde ou commencez à zéro.",
  es="El archivo existente se conserva en la carpeta de copias si restaura una copia de seguridad o empieza desde cero.")
E("setup.error.no_data", en="No Qupil 1.x data was found in “%1”.",
  de="In „%1“ wurden keine Qupil-1.x-Daten gefunden.", fr="Aucune donnée Qupil 1.x n’a été trouvée dans « %1 ».",
  es="No se han encontrado datos de Qupil 1.x en “%1”.", c="%1 = folder")
E("setup.error.read", en="“%1” could not be read. %2", de="„%1“ konnte nicht gelesen werden. %2",
  fr="Impossible de lire « %1 ». %2", es="No se ha podido leer “%1”. %2",
  c="%1 = file, %2 = technical reason (may be empty)")
E("setup.error.unknown", en="“%1” contains neither Qupil 1.x data nor a Qupil backup.",
  de="„%1“ enthält weder Qupil-1.x-Daten noch ein Qupil-Backup.",
  fr="« %1 » ne contient ni des données Qupil 1.x ni une sauvegarde Qupil.",
  es="“%1” no contiene datos de Qupil 1.x ni una copia de seguridad de Qupil.", c="%1 = file")
E("setup.error.failed", en="The data could not be taken over: %1",
  de="Die Daten konnten nicht übernommen werden: %1", fr="Les données n’ont pas pu être reprises : %1",
  es="No se han podido importar los datos: %1", c="%1 = technical error message")
E("takeover.failed", en="Take-over failed", de="Übernahme fehlgeschlagen", fr="Échec de la reprise",
  es="Error en la importación")
E("summary.title.legacy", en="Take-over complete", de="Übernahme abgeschlossen", fr="Reprise terminée",
  es="Importación completada")
E("summary.title.backup", en="Backup restored", de="Backup zurückgespielt", fr="Sauvegarde restaurée",
  es="Copia de seguridad restaurada")
E("summary.check_hint", en="Please check your data briefly – then off you go.",
  de="Bitte werfen Sie einen kurzen Blick auf Ihre Daten – dann kann es losgehen.",
  fr="Jetez un rapide coup d’œil à vos données – et c’est parti.",
  es="Revise brevemente sus datos y listo para empezar.")
E("summary.source", en="From: %1", de="Aus: %1", fr="Source : %1", es="Origen: %1", c="%1 = folder or file")
E("summary.settings", en="Lists and settings (styles, locations, instruments) were taken over as well.",
  de="Listen und Einstellungen (Stilrichtungen, Unterrichtsorte, Instrumente) wurden ebenfalls übernommen.",
  fr="Les listes et paramètres (styles, lieux, instruments) ont également été repris.",
  es="También se han importado las listas y los ajustes (estilos, lugares, instrumentos).")
E("summary.step.lessons", en="Lessons", de="Unterrichte", fr="Cours", es="Clases",
  c="Progress step while importing")
E("summary.students", src="%n student(s)", en=["%n student", "%n students"],
  de=["%n Schüler", "%n Schüler"], fr=["%n élève", "%n élèves"], es=["%n alumno", "%n alumnos"])
E("summary.archived_students", src="%n archived student(s)",
  en=["%n archived student", "%n archived students"],
  de=["%n archivierter Schüler", "%n archivierte Schüler"],
  fr=["%n élève archivé", "%n élèves archivés"], es=["%n alumno archivado", "%n alumnos archivados"])
E("summary.lessons", src="%n lesson(s)", en=["%n lesson", "%n lessons"],
  de=["%n Unterricht", "%n Unterrichte"], fr=["%n cours", "%n cours"], es=["%n clase", "%n clases"])
E("summary.notes", src="%n note(s)", en=["%n note", "%n notes"], de=["%n Notiz", "%n Notizen"],
  fr=["%n note", "%n notes"], es=["%n nota", "%n notas"])
E("summary.pieces", src="%n piece(s)", en=["%n piece", "%n pieces"], de=["%n Werk", "%n Werke"],
  fr=["%n morceau", "%n morceaux"], es=["%n obra", "%n obras"])
E("summary.activities", src="%n activity/activities", en=["%n activity", "%n activities"],
  de=["%n Aktivität", "%n Aktivitäten"], fr=["%n activité", "%n activités"],
  es=["%n actividad", "%n actividades"])
E("summary.events", src="%n event(s)", en=["%n event", "%n events"],
  de=["%n Veranstaltung", "%n Veranstaltungen"], fr=["%n événement", "%n événements"],
  es=["%n evento", "%n eventos"])
E("summary.archived_events", src="%n archived event(s)", en=["%n archived event", "%n archived events"],
  de=["%n archivierte Veranstaltung", "%n archivierte Veranstaltungen"],
  fr=["%n événement archivé", "%n événements archivés"], es=["%n evento archivado", "%n eventos archivados"])
E("summary.library", src="%n item(s) in the sheet music library",
  en=["%n item in the sheet music library", "%n items in the sheet music library"],
  de=["%n Eintrag in der Noten-Bibliothek", "%n Einträge in der Noten-Bibliothek"],
  fr=["%n partition dans la bibliothèque", "%n partitions dans la bibliothèque"],
  es=["%n partitura en la biblioteca", "%n partituras en la biblioteca"])
E("summary.reminders", src="%n reminder(s)", en=["%n reminder", "%n reminders"],
  de=["%n Erinnerung", "%n Erinnerungen"], fr=["%n rappel", "%n rappels"],
  es=["%n recordatorio", "%n recordatorios"])
E("summary.instruments", src="%n instrument(s)", en=["%n instrument", "%n instruments"],
  de=["%n Instrument", "%n Instrumente"], fr=["%n instrument", "%n instruments"],
  es=["%n instrumento", "%n instrumentos"])
E("summary.skipped", src="%n entry/entries without student or lesson skipped",
  en=["%n entry whose student or lesson no longer existed was skipped.",
      "%n entries whose student or lesson no longer existed were skipped."],
  de=["%n Eintrag ohne vorhandenen Schüler oder Unterricht wurde übersprungen.",
      "%n Einträge ohne vorhandenen Schüler oder Unterricht wurden übersprungen."],
  fr=["%n entrée dont l’élève ou le cours n’existait plus a été ignorée.",
      "%n entrées dont l’élève ou le cours n’existait plus ont été ignorées."],
  es=["Se ha omitido %n entrada cuyo alumno o clase ya no existía.",
      "Se han omitido %n entradas cuyo alumno o clase ya no existía."])
E("summary.unparsed_dates", src="%n date(s) in an unknown format kept as text",
  en=["%n date had an unknown format and was kept as text.",
      "%n dates had an unknown format and were kept as text."],
  de=["%n Datum hatte ein unbekanntes Format und wurde als Text übernommen.",
      "%n Datumsangaben hatten ein unbekanntes Format und wurden als Text übernommen."],
  fr=["%n date avait un format inconnu et a été conservée comme texte.",
      "%n dates avaient un format inconnu et ont été conservées comme texte."],
  es=["%n fecha tenía un formato desconocido y se ha conservado como texto.",
      "%n fechas tenían un formato desconocido y se han conservado como texto."])
E("data.replace_warning",
  en="The data currently in Qupil 2 will be replaced by the data taken over.",
  de="Die aktuellen Daten in Qupil 2 werden durch die übernommenen Daten ersetzt.",
  fr="Les données actuelles de Qupil 2 seront remplacées par les données reprises.",
  es="Los datos actuales de Qupil 2 se sustituirán por los datos importados.")
E("data.safety_backup", en="Your current data is backed up automatically beforehand.",
  de="Ihre aktuellen Daten werden vorher automatisch gesichert.",
  fr="Vos données actuelles sont sauvegardées automatiquement au préalable.",
  es="Antes se hace automáticamente una copia de seguridad de sus datos actuales.")
E("data.imported_at", en="Taken over from Qupil 1.x on %1", de="Aus Qupil 1.x übernommen am %1",
  fr="Repris de Qupil 1.x le %1", es="Importado de Qupil 1.x el %1", c="%1 = date and time")
E("data.restored_at", en="Restored from a backup on %1", de="Aus einem Backup zurückgespielt am %1",
  fr="Restauré à partir d’une sauvegarde le %1", es="Restaurado desde una copia de seguridad el %1",
  c="%1 = date and time")
E("data.last_auto_backup", en="Last automatic backup: %1", de="Letztes automatisches Backup: %1",
  fr="Dernière sauvegarde automatique : %1", es="Última copia automática: %1", c="%1 = date and time")
E("data.auto_backup_info", en="Qupil makes an automatic backup once a day and keeps the last 14.",
  de="Qupil erstellt einmal täglich ein automatisches Backup und behält die letzten 14.",
  fr="Qupil effectue une sauvegarde automatique par jour et conserve les 14 dernières.",
  es="Qupil hace una copia automática al día y conserva las 14 últimas.")
E("data.location", en="Stored in %1", de="Gespeichert in %1", fr="Enregistré dans %1", es="Guardado en %1",
  c="%1 = path of the database file")
E("data.backup_now", en="Create backup…", de="Backup erstellen ...", fr="Créer une sauvegarde…",
  es="Crear copia de seguridad…")
E("data.restore", en="Restore backup…", de="Backup zurückspielen ...", fr="Restaurer une sauvegarde…",
  es="Restaurar copia de seguridad…")
E("data.take_over", en="Take over from Qupil 1.x…", de="Aus Qupil 1.x übernehmen ...",
  fr="Reprendre de Qupil 1.x…", es="Importar de Qupil 1.x…")
E("data.open_backup_folder", en="Open backup folder", de="Backup-Ordner öffnen",
  fr="Ouvrir le dossier des sauvegardes", es="Abrir carpeta de copias")
E("setup.demo.action", en="Try it out with sample data", de="Mit Beispieldaten ausprobieren",
  fr="Essayer avec des données d’exemple", es="Probar con datos de ejemplo")
E("summary.title.demo", en="Sample data loaded", de="Beispieldaten geladen", fr="Données d’exemple chargées",
  es="Datos de ejemplo cargados")
E("summary.demo_done", en="Invented students, lessons and events to try everything out.",
  de="Erfundene Schüler, Unterrichte und Veranstaltungen zum Ausprobieren.",
  fr="Des élèves, cours et événements fictifs pour tout essayer.",
  es="Alumnos, clases y eventos inventados para probarlo todo.")
E("data.demo",
  en="You are working with sample data. Take over your own data whenever you like – it replaces the sample data.",
  de="Sie arbeiten mit Beispieldaten. Übernehmen Sie Ihre eigenen Daten, wann immer Sie möchten – sie ersetzen die Beispieldaten.",
  fr="Vous travaillez avec des données d’exemple. Reprenez vos propres données quand vous le souhaitez – elles remplacent les données d’exemple.",
  es="Está trabajando con datos de ejemplo. Importe sus propios datos cuando quiera: sustituyen a los datos de ejemplo.")
E("backup.restore.action", en="Restore", de="Zurückspielen", fr="Restaurer", es="Restaurar")
E("common.filter_all", en="All files (*)", de="Alle Dateien (*)", fr="Tous les fichiers (*)",
  es="Todos los archivos (*)", c="File dialog filter. Keep the pattern in parentheses unchanged.")

# ---------------------------------------------------------------- screens of Qupil 2 (new)
E("common.saved", en="Saved", de="Gespeichert", fr="Enregistré", es="Guardado")
E("time.today", en="Today", de="Heute", fr="Aujourd’hui", es="Hoy")
E("time.yesterday", en="Yesterday", de="Gestern", fr="Hier", es="Ayer")
E("time.days_ago", src="%n day(s) ago", en=["%n day ago", "%n days ago"], de=["vor %n Tag", "vor %n Tagen"],
  fr=["il y a %n jour", "il y a %n jours"], es=["hace %n día", "hace %n días"])
E("time.weeks_ago", src="%n week(s) ago", en=["%n week ago", "%n weeks ago"], de=["vor %n Woche", "vor %n Wochen"],
  fr=["il y a %n semaine", "il y a %n semaines"], es=["hace %n semana", "hace %n semanas"])
E("time.in_days", src="in %n day(s)", en=["in %n day", "in %n days"], de=["in %n Tag", "in %n Tagen"],
  fr=["dans %n jour", "dans %n jours"], es=["en %n día", "en %n días"])
E("calendar.previous_month", en="Previous month", de="Vorheriger Monat", fr="Mois précédent", es="Mes anterior")
E("calendar.next_month", en="Next month", de="Nächster Monat", fr="Mois suivant", es="Mes siguiente")
E("calendar.previous_year", en="Previous year", de="Vorheriges Jahr", fr="Année précédente", es="Año anterior")
E("calendar.next_year", en="Next year", de="Nächstes Jahr", fr="Année suivante", es="Año siguiente")
E("calendar.clear", en="No date", de="Kein Datum", fr="Aucune date", es="Sin fecha")
E("today.birthday_on", en="Birthday on %1", de="Geburtstag am %1", fr="Anniversaire le %1", es="Cumpleaños el %1",
  c="%1 = weekday and date")
E("today.reminder", en="Reminder", de="Erinnerung", fr="Rappel", es="Recordatorio")
E("today.running", en="Now", de="Läuft", fr="En cours", es="En curso", c="Badge on the lesson that is running now")
E("today.pieces_in_progress", src="%n piece(s) in progress", en=["%n piece in progress", "%n pieces in progress"],
  de=["%n Werk in Arbeit", "%n Werke in Arbeit"], fr=["%n morceau en cours", "%n morceaux en cours"],
  es=["%n obra en curso", "%n obras en curso"])
E("today.pieces_ready", src="%n ready to perform", en=["%n ready to perform", "%n ready to perform"],
  de=["%n vorspielreif", "%n vorspielreif"], fr=["%n prêt à jouer", "%n prêts à jouer"],
  es=["%n lista para tocar", "%n listas para tocar"])
E("schedule.new_student", en="New student", de="Neuer Schüler", fr="Nouvel élève", es="Nuevo alumno")
E("schedule.new_lesson", en="New lesson", de="Neuer Unterricht", fr="Nouveau cours", es="Nueva clase")
E("schedule.day_lessons", en="Lessons", de="Unterricht", fr="Cours", es="Clases")
E("schedule.no_lessons_day", en="No lessons on this day", de="An diesem Tag kein Unterricht",
  fr="Pas de cours ce jour-là", es="No hay clases este día")
E("schedule.pick_hint", en="Choose a lesson or a student.", de="Wählen Sie einen Unterricht oder einen Schüler.",
  fr="Choisissez un cours ou un élève.", es="Elija una clase o un alumno.")
E("students.pick_hint", en="Choose a student.", de="Wählen Sie einen Schüler.", fr="Choisissez un élève.",
  es="Elija un alumno.")
E("students.empty", en="No students yet. Add one with +.", de="Noch keine Schüler. Mit + legen Sie einen an.",
  fr="Aucun élève pour l’instant. Ajoutez-en un avec +.", es="Todavía no hay alumnos. Añada uno con +.")
E("students.no_match", en="No student matches the search.", de="Kein Schüler passt zur Suche.",
  fr="Aucun élève ne correspond à la recherche.", es="Ningún alumno coincide con la búsqueda.")
E("students.no_lesson_short", en="no lesson", de="kein Unterricht", fr="aucun cours", es="sin clase")
E("student.inactive_lessons", src="+%n inactive", en=["+%n inactive", "+%n inactive"], de=["+%n inaktiv", "+%n inaktiv"],
  fr=["+%n inactif", "+%n inactifs"], es=["+%n inactiva", "+%n inactivas"], c="Earlier lessons of a student")
E("student.no_lesson", en="This student is not in any lesson yet. Add the student to a lesson to write notes and pieces.",
  de="Dieser Schüler ist noch in keinem Unterricht. Fügen Sie ihn einem Unterricht hinzu, um Notizen und Werke anzulegen.",
  fr="Cet élève ne participe encore à aucun cours. Ajoutez-le à un cours pour saisir des notes et des morceaux.",
  es="Este alumno aún no está en ninguna clase. Añádalo a una clase para escribir notas y obras.")
E("student.new_hint", en="Everything else (address, parents, rental instrument) is under “Data” on the student’s page.",
  de="Alles Weitere (Adresse, Eltern, Leihinstrument) finden Sie beim Schüler unter „Daten“.",
  fr="Le reste (adresse, parents, instrument loué) se trouve dans « Données » sur la page de l’élève.",
  es="Todo lo demás (dirección, padres, instrumento prestado) está en «Datos» en la página del alumno.")
E("student.add_reminder", en="Set up reminder…", de="Erinnerung einrichten ...", fr="Créer un rappel…",
  es="Crear recordatorio…")
E("notes.empty", en="No notes yet.", de="Noch keine Notizen.", fr="Aucune note pour l’instant.",
  es="Todavía no hay notas.")
E("note.edit", en="Edit note", de="Notiz bearbeiten", fr="Modifier la note", es="Editar nota")
E("pieces.empty", en="No pieces yet.", de="Noch keine Werke.", fr="Aucun morceau pour l’instant.",
  es="Todavía no hay obras.")
E("piece.edit", en="Edit piece", de="Werk bearbeiten", fr="Modifier le morceau", es="Editar obra")
E("loans.empty", en="No borrowed sheet music.", de="Keine ausgeliehenen Noten.", fr="Aucune partition empruntée.",
  es="No hay partituras prestadas.")
E("lesson.edit", en="Edit lesson", de="Unterricht bearbeiten", fr="Modifier le cours", es="Editar clase")
E("lesson.duration", en="%1 minutes", de="%1 Minuten", fr="%1 minutes", es="%1 minutos", c="%1 = lesson length")
E("lesson.name.auto_short", en="Automatic", de="Automatisch", fr="Automatique", es="Automático")
E("lesson.name.manual_short", en="Own name", de="Eigener Name", fr="Nom libre", es="Nombre propio")
E("lesson.no_students", en="No students in this lesson yet.", de="Noch keine Schüler in diesem Unterricht.",
  fr="Aucun élève dans ce cours pour l’instant.", es="Todavía no hay alumnos en esta clase.")
E("lesson.note_for_all", en="Note for everybody", de="Notiz für alle", fr="Note pour tous", es="Nota para todos")
E("events.empty", en="No events yet.", de="Noch keine Veranstaltungen.", fr="Aucun événement pour l’instant.",
  es="Todavía no hay eventos.")
E("events.pick_hint", en="Choose an event.", de="Wählen Sie eine Veranstaltung.", fr="Choisissez un événement.",
  es="Elija un evento.")
E("event.programme", en="Programme", de="Programm", fr="Programme", es="Programa")
E("event.programme_empty", en="No contributions yet. Add pieces that are ready to perform.",
  de="Noch keine Beiträge. Fügen Sie vorspielreife Werke hinzu.",
  fr="Aucune contribution pour l’instant. Ajoutez des morceaux prêts à jouer.",
  es="Todavía no hay intervenciones. Añada obras listas para tocar.")
E("event.label.contributions", en="Contributions:", de="Beiträge:", fr="Contributions :", es="Intervenciones:")
E("event.contributions", src="%n contribution(s)", en=["%n contribution", "%n contributions"],
  de=["%n Beitrag", "%n Beiträge"], fr=["%n contribution", "%n contributions"],
  es=["%n intervención", "%n intervenciones"])
E("event.external_count", src="%n external", en=["%n external", "%n external"], de=["%n extern", "%n extern"],
  fr=["%n externe", "%n externes"], es=["%n externa", "%n externas"])
E("event.add_ready", en="Ready piece", de="Vorspielreifes Werk", fr="Morceau prêt", es="Obra lista")
E("event.no_ready_pieces", en="No pieces are marked as “ready to perform” at the moment.",
  de="Zurzeit ist kein Werk als „vorspielreif“ markiert.",
  fr="Aucun morceau n’est actuellement marqué « prêt à jouer ».",
  es="Por ahora ninguna obra está marcada como «lista para tocar».")
E("event.move_up", en="Move up", de="Nach oben", fr="Monter", es="Subir")
E("event.move_down", en="Move down", de="Nach unten", fr="Descendre", es="Bajar")
E("metronome.pitch_down", en="Lower pitch", de="Tiefer stimmen", fr="Diapason plus bas", es="Afinación más baja")
E("metronome.pitch_up", en="Higher pitch", de="Höher stimmen", fr="Diapason plus haut", es="Afinación más alta")
E("more.birthdays_today", src="%n today", en=["%n today", "%n today"], de=["%n heute", "%n heute"],
  fr=["%n aujourd’hui", "%n aujourd’hui"], es=["%n hoy", "%n hoy"])
E("birthday.none_soon", en="No birthdays in the next weeks.", de="In den nächsten Wochen hat niemand Geburtstag.",
  fr="Aucun anniversaire dans les prochaines semaines.", es="No hay cumpleaños en las próximas semanas.")
E("reminders.empty", en="No reminders.", de="Keine Erinnerungen.", fr="Aucun rappel.", es="No hay recordatorios.")
E("settings.lists", en="Lists", de="Listen", fr="Listes", es="Listas")

# ---------------------------------------------------------------- sync (new)
E("sync.title", en="Sync", de="Synchronisation", fr="Synchronisation", es="Sincronización")
E("sync.all_synced", en="Everything is in sync", de="Alles synchron", fr="Tout est synchronisé",
  es="Todo está sincronizado")
E("sync.now", en="Sync now", de="Jetzt synchronisieren", fr="Synchroniser maintenant",
  es="Sincronizar ahora")
E("sync.in_progress", en="Syncing…", de="Synchronisiere ...", fr="Synchronisation…", es="Sincronizando…")
E("sync.failed", en="Sync failed: %1", de="Synchronisation fehlgeschlagen: %1",
  fr="Échec de la synchronisation : %1", es="Error de sincronización: %1", c="%1 = error text")
E("sync.last", en="Last sync: %1", de="Zuletzt synchronisiert: %1", fr="Dernière synchronisation : %1",
  es="Última sincronización: %1", c="%1 = relative time, e.g. '3 minutes ago'")
E("sync.minutes_ago", src="%n minute(s) ago",
  en=["%n minute ago", "%n minutes ago"], de=["vor %n Minute", "vor %n Minuten"],
  fr=["il y a %n minute", "il y a %n minutes"], es=["hace %n minuto", "hace %n minutos"])
E("sync.pair", en="Pair device", de="Gerät koppeln", fr="Associer un appareil", es="Vincular dispositivo")
E("sync.devices", en="Devices", de="Geräte", fr="Appareils", es="Dispositivos")
E("sync.device.this", en="This device", de="Dieses Gerät", fr="Cet appareil", es="Este dispositivo")
E("sync.nextcloud.connect", en="Connect to Nextcloud", de="Mit Nextcloud verbinden",
  fr="Se connecter à Nextcloud", es="Conectar con Nextcloud")
E("sync.nextcloud.server", en="Server address", de="Serveradresse", fr="Adresse du serveur",
  es="Dirección del servidor")
E("sync.nextcloud.login_browser", en="Sign in in the browser to give Qupil access.",
  de="Melden Sie sich im Browser an, um Qupil den Zugriff zu erlauben.",
  fr="Connectez-vous dans le navigateur pour autoriser Qupil à accéder à votre compte.",
  es="Inicie sesión en el navegador para dar acceso a Qupil.")
E("sync.wifi", en="Wi-Fi sync", de="WLAN-Sync", fr="Synchronisation Wi-Fi", es="Sincronización por Wi-Fi")
E("sync.e2e", en="End-to-end encrypted", de="Ende-zu-Ende-verschlüsselt", fr="Chiffré de bout en bout",
  es="Cifrado de extremo a extremo")
E("sync.local_network_permission",
  en="Qupil needs access to the local network to sync with your other devices over Wi-Fi.",
  de="Qupil benötigt Zugriff auf das lokale Netzwerk, um per WLAN mit Ihren anderen Geräten zu synchronisieren.",
  fr="Qupil a besoin d’accéder au réseau local pour se synchroniser en Wi-Fi avec vos autres appareils.",
  es="Qupil necesita acceso a la red local para sincronizarse por Wi-Fi con sus otros dispositivos.",
  c="iOS permission text (NSLocalNetworkUsageDescription).")
E("sync.status.off", en="Off – your data is only on this device", de="Aus – Ihre Daten sind nur auf diesem Gerät",
  fr="Désactivée – vos données sont uniquement sur cet appareil",
  es="Desactivada: sus datos solo están en este dispositivo")
E("sync.setup_short", en="Set up", de="Einrichten", fr="Configurer", es="Configurar")
E("sync.status.problem", en="Sync problem", de="Problem bei der Synchronisation", fr="Problème de synchronisation",
  es="Problema de sincronización")
E("sync.never", en="Not synced yet", de="Noch nicht synchronisiert", fr="Pas encore synchronisé",
  es="Aún no sincronizado")
E("sync.just_now", en="just now", de="gerade eben", fr="à l’instant", es="ahora mismo")
E("sync.today_at", en="today %1", de="heute %1", fr="aujourd’hui %1", es="hoy %1", c="%1 = time, e.g. 13:05")
E("sync.nextcloud.when", en="Nextcloud %1", de="Nextcloud %1", fr="Nextcloud %1", es="Nextcloud %1",
  c="%1 = when, e.g. '3 minutes ago' or 'today 13:05'")
E("sync.intro",
  en="Keep Qupil up to date on all your devices – through your own Nextcloud or directly over Wi-Fi. "
     "Your data is encrypted before it leaves the device.",
  de="Halten Sie Qupil auf all Ihren Geräten auf dem gleichen Stand – über Ihre eigene Nextcloud oder direkt "
     "per WLAN. Ihre Daten werden verschlüsselt, bevor sie das Gerät verlassen.",
  fr="Gardez Qupil à jour sur tous vos appareils – via votre propre Nextcloud ou directement en Wi-Fi. "
     "Vos données sont chiffrées avant de quitter l’appareil.",
  es="Mantenga Qupil al día en todos sus dispositivos, a través de su propio Nextcloud o directamente por Wi-Fi. "
     "Sus datos se cifran antes de salir del dispositivo.")
E("sync.nextcloud.intro",
  en="Qupil stores its data encrypted in a folder named “Qupil” in your Nextcloud. You sign in in the browser – "
     "Qupil never sees your Nextcloud password.",
  de="Qupil legt seine Daten verschlüsselt im Ordner „Qupil“ Ihrer Nextcloud ab. Sie melden sich im Browser an – "
     "Ihr Nextcloud-Passwort sieht Qupil nie.",
  fr="Qupil enregistre ses données chiffrées dans un dossier « Qupil » de votre Nextcloud. Vous vous connectez "
     "dans le navigateur – Qupil ne voit jamais votre mot de passe Nextcloud.",
  es="Qupil guarda sus datos cifrados en una carpeta «Qupil» de su Nextcloud. Usted inicia sesión en el "
     "navegador: Qupil nunca ve su contraseña de Nextcloud.")
E("sync.nextcloud.waiting", en="Waiting for the sign-in in the browser …",
  de="Warte auf die Anmeldung im Browser ...", fr="En attente de la connexion dans le navigateur…",
  es="Esperando el inicio de sesión en el navegador…")
E("sync.nextcloud.connected_as", en="Signed in as %1 on %2", de="Angemeldet als %1 auf %2",
  fr="Connecté en tant que %1 sur %2", es="Sesión iniciada como %1 en %2", c="%1 = user name, %2 = server")
E("sync.nextcloud.disconnect", en="Disconnect", de="Verbindung trennen", fr="Se déconnecter", es="Desconectar")
E("sync.nextcloud.disconnect.confirm",
  en="Disconnect from Nextcloud? The data on this device stays, and your other devices are not affected.",
  de="Verbindung zur Nextcloud trennen? Die Daten auf diesem Gerät bleiben erhalten, Ihre anderen Geräte sind "
     "nicht betroffen.",
  fr="Se déconnecter de Nextcloud ? Les données de cet appareil sont conservées et vos autres appareils ne sont "
     "pas concernés.",
  es="¿Desconectar de Nextcloud? Los datos de este dispositivo se conservan y sus otros dispositivos no se ven "
     "afectados.")
E("sync.checking", en="Checking Nextcloud …", de="Prüfe Nextcloud ...", fr="Vérification de Nextcloud…",
  es="Comprobando Nextcloud…")
E("sync.password", en="Sync password", de="Sync-Passwort", fr="Mot de passe de synchronisation",
  es="Contraseña de sincronización")
E("sync.password.repeat", en="Repeat password", de="Passwort wiederholen", fr="Répéter le mot de passe",
  es="Repetir la contraseña")
E("sync.password.choose_title", en="Choose a sync password", de="Sync-Passwort festlegen",
  fr="Choisir un mot de passe de synchronisation", es="Elegir una contraseña de sincronización")
E("sync.password.choose_text",
  en="Your data is encrypted with this password before it leaves the device – not even the operator of the "
     "Nextcloud can read it. You need it on each of your other devices. It cannot be recovered, so keep it in a "
     "safe place.",
  de="Mit diesem Passwort werden Ihre Daten verschlüsselt, bevor sie das Gerät verlassen – selbst der Betreiber "
     "der Nextcloud kann sie nicht lesen. Sie brauchen es auf jedem weiteren Gerät. Es lässt sich nicht "
     "wiederherstellen, bewahren Sie es also gut auf.",
  fr="Vos données sont chiffrées avec ce mot de passe avant de quitter l’appareil – même l’opérateur du Nextcloud "
     "ne peut pas les lire. Il vous le faudra sur chacun de vos autres appareils. Il ne peut pas être récupéré : "
     "conservez-le en lieu sûr.",
  es="Sus datos se cifran con esta contraseña antes de salir del dispositivo: ni siquiera el operador del "
     "Nextcloud puede leerlos. La necesitará en cada uno de sus otros dispositivos. No se puede recuperar, así que "
     "guárdela en un lugar seguro.")
E("sync.password.enter_title", en="Enter the sync password", de="Sync-Passwort eingeben",
  fr="Saisir le mot de passe de synchronisation", es="Introducir la contraseña de sincronización")
E("sync.password.enter_text",
  en="This Nextcloud already holds Qupil data. Enter the sync password you chose on your first device.",
  de="In dieser Nextcloud liegen bereits Qupil-Daten. Geben Sie das Sync-Passwort ein, das Sie auf Ihrem ersten "
     "Gerät festgelegt haben.",
  fr="Ce Nextcloud contient déjà des données Qupil. Saisissez le mot de passe de synchronisation choisi sur votre "
     "premier appareil.",
  es="Este Nextcloud ya contiene datos de Qupil. Introduzca la contraseña de sincronización que eligió en su "
     "primer dispositivo.")
E("sync.password.too_short", en="At least 8 characters", de="Mindestens 8 Zeichen", fr="Au moins 8 caractères",
  es="Al menos 8 caracteres")
E("sync.password.mismatch", en="The passwords do not match", de="Die Passwörter stimmen nicht überein",
  fr="Les mots de passe ne correspondent pas", es="Las contraseñas no coinciden")
E("sync.password.show", en="Show password", de="Passwort anzeigen", fr="Afficher le mot de passe",
  es="Mostrar la contraseña")
E("sync.password.set", en="Save password and sync", de="Passwort festlegen und synchronisieren",
  fr="Enregistrer et synchroniser", es="Guardar y sincronizar")
E("sync.password.unlock", en="Unlock and sync", de="Entsperren und synchronisieren",
  fr="Déverrouiller et synchroniser", es="Desbloquear y sincronizar")
E("sync.join.title", en="The data on this device", de="Die Daten auf diesem Gerät", fr="Les données de cet appareil",
  es="Los datos de este dispositivo")
E("sync.join.replace", en="Replace with the data from Nextcloud", de="Durch die Daten aus der Nextcloud ersetzen",
  fr="Remplacer par les données du Nextcloud", es="Reemplazar por los datos del Nextcloud")
E("sync.join.replace.hint", en="Recommended. A backup of the data on this device is kept.",
  de="Empfohlen. Die Daten auf diesem Gerät werden vorher gesichert.",
  fr="Recommandé. Une sauvegarde des données de cet appareil est conservée.",
  es="Recomendado. Se guarda una copia de seguridad de los datos de este dispositivo.")
E("sync.join.merge", en="Merge with the data from Nextcloud", de="Mit den Daten aus der Nextcloud zusammenführen",
  fr="Fusionner avec les données du Nextcloud", es="Combinar con los datos del Nextcloud")
E("sync.join.merge.hint",
  en="Only if this data does not exist anywhere else – the same data twice (e.g. taken over from Qupil 1.x on two "
     "devices) leads to duplicate entries.",
  de="Nur wenn diese Daten sonst nirgends vorhanden sind – dieselben Daten doppelt (z. B. auf zwei Geräten aus "
     "Qupil 1.x übernommen) führen zu doppelten Einträgen.",
  fr="Seulement si ces données n’existent nulle part ailleurs – les mêmes données en double (p. ex. reprises de "
     "Qupil 1.x sur deux appareils) créent des doublons.",
  es="Solo si estos datos no existen en ningún otro lugar: los mismos datos dos veces (p. ej. importados de "
     "Qupil 1.x en dos dispositivos) generan entradas duplicadas.")
E("sync.join.start_over", en="Start over with the data on this device",
  de="Mit den Daten auf diesem Gerät neu beginnen", fr="Recommencer avec les données de cet appareil",
  es="Empezar de nuevo con los datos de este dispositivo")
E("sync.join.start_over.hint",
  en="The existing folder in Nextcloud is kept under a new name. Choose a new sync password; your other devices "
     "will ask for it.",
  de="Der bisherige Ordner in der Nextcloud bleibt unter neuem Namen erhalten. Legen Sie ein neues Sync-Passwort "
     "fest; Ihre anderen Geräte fragen danach.",
  fr="Le dossier existant du Nextcloud est conservé sous un autre nom. Choisissez un nouveau mot de passe ; vos "
     "autres appareils le demanderont.",
  es="La carpeta existente del Nextcloud se conserva con otro nombre. Elija una nueva contraseña de "
     "sincronización; sus otros dispositivos se la pedirán.")
E("sync.join.start_over.action", en="Start over and sync", de="Neu beginnen und synchronisieren",
  fr="Recommencer et synchroniser", es="Empezar de nuevo y sincronizar")
E("sync.auto", en="Sync automatically", de="Automatisch synchronisieren", fr="Synchroniser automatiquement",
  es="Sincronizar automáticamente")
E("sync.auto.hint", en="At start, shortly after changes and every 10 minutes.",
  de="Beim Start, kurz nach Änderungen und alle 10 Minuten.",
  fr="Au démarrage, peu après les modifications et toutes les 10 minutes.",
  es="Al iniciar, poco después de los cambios y cada 10 minutos.")
E("sync.device_name", en="Name of this device", de="Name dieses Geräts", fr="Nom de cet appareil",
  es="Nombre de este dispositivo")
E("sync.devices.none", en="No other devices yet.", de="Noch keine weiteren Geräte.",
  fr="Aucun autre appareil pour l’instant.", es="Aún no hay otros dispositivos.")
E("sync.error.server", en="No Nextcloud found at %1.", de="Unter %1 wurde keine Nextcloud gefunden.",
  fr="Aucun Nextcloud trouvé à l’adresse %1.", es="No se encontró ningún Nextcloud en %1.", c="%1 = server address")
E("sync.error.login_timeout", en="The sign-in in the browser took too long. Please try again.",
  de="Die Anmeldung im Browser hat zu lange gedauert. Bitte versuchen Sie es noch einmal.",
  fr="La connexion dans le navigateur a pris trop de temps. Veuillez réessayer.",
  es="El inicio de sesión en el navegador tardó demasiado. Inténtelo de nuevo.")
E("sync.error.access_denied", en="Nextcloud denied access. Please disconnect and connect again.",
  de="Die Nextcloud hat den Zugriff verweigert. Bitte trennen Sie die Verbindung und verbinden Sie sich neu.",
  fr="Nextcloud a refusé l’accès. Veuillez vous déconnecter puis vous reconnecter.",
  es="Nextcloud denegó el acceso. Desconéctese y vuelva a conectarse.")
E("sync.error.replace", en="The data on this device could not be replaced.",
  de="Die Daten auf diesem Gerät konnten nicht ersetzt werden.",
  fr="Les données de cet appareil n’ont pas pu être remplacées.",
  es="No se pudieron reemplazar los datos de este dispositivo.")
E("sync.wrong_password", en="The sync password is wrong.", de="Das Sync-Passwort ist falsch.",
  fr="Le mot de passe de synchronisation est incorrect.", es="La contraseña de sincronización es incorrecta.")
E("sync.password_changed", en="The sync password was changed on another device. Please enter the new one.",
  de="Das Sync-Passwort wurde auf einem anderen Gerät geändert. Bitte geben Sie das neue ein.",
  fr="Le mot de passe de synchronisation a été modifié sur un autre appareil. Veuillez saisir le nouveau.",
  es="La contraseña de sincronización se cambió en otro dispositivo. Introduzca la nueva.")
E("sync.folder_missing", en="The Qupil folder in Nextcloud is gone. Choose a sync password to set it up again.",
  de="Der Qupil-Ordner in der Nextcloud ist nicht mehr da. Legen Sie ein Sync-Passwort fest, um ihn neu "
     "einzurichten.",
  fr="Le dossier Qupil du Nextcloud a disparu. Choisissez un mot de passe pour le recréer.",
  es="La carpeta Qupil del Nextcloud ya no existe. Elija una contraseña de sincronización para volver a crearla.")
E("sync.paused_after_replace",
  en="Sync is paused because the data on this device was replaced. Enter the sync password again.",
  de="Die Synchronisation ist pausiert, weil die Daten auf diesem Gerät ersetzt wurden. Geben Sie das "
     "Sync-Passwort erneut ein.",
  fr="La synchronisation est en pause car les données de cet appareil ont été remplacées. Saisissez à nouveau le "
     "mot de passe de synchronisation.",
  es="La sincronización está en pausa porque se reemplazaron los datos de este dispositivo. Vuelva a introducir "
     "la contraseña de sincronización.")
E("sync.waiting_for_data", en="This device is still empty and gets its data with the first sync.",
  de="Dieses Gerät ist noch leer und erhält seine Daten bei der ersten Synchronisation.",
  fr="Cet appareil est encore vide et recevra ses données lors de la première synchronisation.",
  es="Este dispositivo aún está vacío y recibirá sus datos en la primera sincronización.")
E("sync.wifi.when", en="Wi-Fi: %1 %2", de="WLAN: %1 %2", fr="Wi-Fi : %1 %2", es="Wi-Fi: %1 %2",
  c="%1 = device name, %2 = when, e.g. 'today 13:05'")
E("wifi.intro",
  en="Devices on the same Wi-Fi sync directly with each other – no server needed. Pair each device once.",
  de="Geräte im selben WLAN gleichen sich direkt miteinander ab – ganz ohne Server. Koppeln Sie jedes Gerät einmal.",
  fr="Les appareils sur le même Wi-Fi se synchronisent directement entre eux – sans serveur. Associez chaque "
     "appareil une fois.",
  es="Los dispositivos en la misma Wi-Fi se sincronizan directamente entre sí, sin servidor. Vincule cada "
     "dispositivo una vez.")
E("wifi.enabled.hint", en="This device can be reached by your paired devices on the Wi-Fi.",
  de="Ihre gekoppelten Geräte erreichen dieses Gerät im WLAN.",
  fr="Vos appareils associés peuvent joindre cet appareil sur le Wi-Fi.",
  es="Sus dispositivos vinculados pueden acceder a este dispositivo en la Wi-Fi.")
E("wifi.code.title", en="Code of this device", de="Code dieses Geräts", fr="Code de cet appareil",
  es="Código de este dispositivo")
E("wifi.code.hint", en="Enter this code on the other device. Both devices need to be on the same Wi-Fi.",
  de="Geben Sie diesen Code auf dem anderen Gerät ein. Beide Geräte müssen im selben WLAN sein.",
  fr="Saisissez ce code sur l’autre appareil. Les deux appareils doivent être sur le même Wi-Fi.",
  es="Introduzca este código en el otro dispositivo. Ambos deben estar en la misma Wi-Fi.")
E("wifi.nearby", en="Devices nearby", de="Geräte in der Nähe", fr="Appareils à proximité",
  es="Dispositivos cercanos")
E("wifi.nearby.searching", en="Looking for devices … Tap “Pair device” on the other device too.",
  de="Suche nach Geräten ... Tippen Sie auch auf dem anderen Gerät auf „Gerät koppeln“.",
  fr="Recherche d’appareils… Touchez aussi « Associer un appareil » sur l’autre appareil.",
  es="Buscando dispositivos… Toque también «Vincular dispositivo» en el otro dispositivo.")
E("wifi.enter_code", en="Enter the code shown on %1", de="Code von %1 eingeben",
  fr="Saisir le code affiché sur %1", es="Introducir el código que muestra %1", c="%1 = device name")
E("wifi.code", en="Code", de="Code", fr="Code", es="Código")
E("wifi.pair", en="Pair", de="Koppeln", fr="Associer", es="Vincular")
E("wifi.connecting", en="Connecting …", de="Verbinde ...", fr="Connexion…", es="Conectando…")
E("wifi.by_address", en="Device not found? Pair by address", de="Gerät nicht gefunden? Per Adresse koppeln",
  fr="Appareil introuvable ? Associer par adresse", es="¿No aparece el dispositivo? Vincular por dirección")
E("wifi.address", en="Address of the other device", de="Adresse des anderen Geräts",
  fr="Adresse de l’autre appareil", es="Dirección del otro dispositivo")
E("wifi.own_address", en="Address of this device: %1", de="Adresse dieses Geräts: %1",
  fr="Adresse de cet appareil : %1", es="Dirección de este dispositivo: %1", c="%1 = IP address(es)")
E("wifi.paired_devices", en="Paired devices", de="Gekoppelte Geräte", fr="Appareils associés",
  es="Dispositivos vinculados")
E("wifi.none", en="No device paired yet.", de="Noch kein Gerät gekoppelt.",
  fr="Aucun appareil associé pour l’instant.", es="Aún no hay ningún dispositivo vinculado.")
E("wifi.online", en="nearby", de="in der Nähe", fr="à proximité", es="cerca")
E("wifi.offline", en="not nearby", de="nicht in der Nähe", fr="pas à proximité", es="no está cerca")
E("wifi.paused", en="Paused – please decide", de="Pausiert – bitte entscheiden", fr="En pause – à vous de choisir",
  es="En pausa: decida")
E("wifi.decide", en="Decide", de="Entscheiden", fr="Choisir", es="Decidir")
E("wifi.problem.unknown", en="No longer knows this device – please pair again",
  de="Kennt dieses Gerät nicht mehr – bitte neu koppeln",
  fr="Ne reconnaît plus cet appareil – veuillez l’associer à nouveau",
  es="Ya no reconoce este dispositivo: vuelva a vincularlo")
E("wifi.forget", en="Unpair", de="Kopplung aufheben", fr="Dissocier", es="Desvincular")
E("wifi.forget.confirm", en="Unpair %1? The data on both devices stays.",
  de="Kopplung mit %1 aufheben? Die Daten auf beiden Geräten bleiben erhalten.",
  fr="Dissocier %1 ? Les données des deux appareils sont conservées.",
  es="¿Desvincular %1? Los datos de ambos dispositivos se conservan.", c="%1 = device name")
E("wifi.join.title", en="Data on both devices", de="Daten auf beiden Geräten", fr="Données sur les deux appareils",
  es="Datos en ambos dispositivos")
E("wifi.join.text", en="%1 and this device both have data. How should they go together?",
  de="Auf %1 und auf diesem Gerät sind Daten. Wie sollen sie zusammenkommen?",
  fr="%1 et cet appareil contiennent tous deux des données. Comment les réunir ?",
  es="%1 y este dispositivo tienen datos. ¿Cómo deben combinarse?", c="%1 = device name")
E("wifi.join.replace", en="Replace the data on this device with that of %1",
  de="Daten auf diesem Gerät durch die von %1 ersetzen",
  fr="Remplacer les données de cet appareil par celles de %1",
  es="Reemplazar los datos de este dispositivo por los de %1", c="%1 = device name")
E("wifi.join.merge", en="Merge", de="Zusammenführen", fr="Fusionner", es="Combinar")
E("wifi.join.action", en="Sync", de="Synchronisieren", fr="Synchroniser", es="Sincronizar")
E("wifi.paired", en="Paired with %1", de="Gekoppelt mit %1", fr="Associé à %1", es="Vinculado con %1",
  c="%1 = device name")
E("wifi.error.gone", en="The device can no longer be seen. Are both on the same Wi-Fi?",
  de="Das Gerät ist nicht mehr zu sehen. Sind beide im selben WLAN?",
  fr="L’appareil n’est plus visible. Les deux sont-ils sur le même Wi-Fi ?",
  es="El dispositivo ya no está visible. ¿Están ambos en la misma Wi-Fi?")
E("wifi.error.address", en="Please enter an address like 192.168.1.23.",
  de="Bitte geben Sie eine Adresse wie 192.168.1.23 ein.", fr="Veuillez saisir une adresse comme 192.168.1.23.",
  es="Introduzca una dirección como 192.168.1.23.")
E("wifi.error.wrong_code", en="The code is not right. The other device now shows a new code.",
  de="Der Code stimmt nicht. Das andere Gerät zeigt jetzt einen neuen Code.",
  fr="Le code n’est pas correct. L’autre appareil affiche maintenant un nouveau code.",
  es="El código no es correcto. El otro dispositivo muestra ahora un código nuevo.")
E("wifi.error.wrong_code_here", en="A wrong code was entered on another device. This device shows a new code now.",
  de="Auf einem anderen Gerät wurde ein falscher Code eingegeben. Dieses Gerät zeigt jetzt einen neuen Code.",
  fr="Un code erroné a été saisi sur un autre appareil. Cet appareil affiche maintenant un nouveau code.",
  es="Se introdujo un código erróneo en otro dispositivo. Este dispositivo muestra ahora un código nuevo.")
E("wifi.error.not_pairing", en="The other device no longer shows a code. Tap “Pair device” there.",
  de="Das andere Gerät zeigt keinen Code mehr. Tippen Sie dort auf „Gerät koppeln“.",
  fr="L’autre appareil n’affiche plus de code. Touchez « Associer un appareil » sur celui-ci.",
  es="El otro dispositivo ya no muestra ningún código. Toque «Vincular dispositivo» en él.")
E("wifi.error.connect", en="Could not reach the other device (%1).",
  de="Keine Verbindung zum anderen Gerät (%1).", fr="Impossible de joindre l’autre appareil (%1).",
  es="No se pudo conectar con el otro dispositivo (%1).", c="%1 = technical reason")
# ---------------------------------------------------------------- library, instruments, archive, documents (new)
E("library.empty", en="No sheet music in the library yet.", de="Noch keine Noten in der Bibliothek.",
  fr="Aucune partition dans la bibliothèque pour l’instant.", es="Aún no hay partituras en la biblioteca.")
E("library.search", en="Search sheet music", de="Noten suchen", fr="Rechercher des partitions",
  es="Buscar partituras")
E("library.new", en="New sheet music", de="Neue Noten", fr="Nouvelle partition", es="Nueva partitura")
E("library.edit", en="Sheet music", de="Noten", fr="Partition", es="Partitura")
E("library.lent_to_since", en="Lent to %1 since %2", de="Verliehen an %1 seit %2", fr="Prêtée à %1 depuis le %2",
  es="Prestada a %1 desde el %2", c="%1 = student, %2 = date")
E("library.lend_to_student", en="Lend to", de="Verleihen an", fr="Prêter à", es="Prestar a")
E("library.lend", en="Lend", de="Verleihen", fr="Prêter", es="Prestar")
E("library.return", en="Returned", de="Zurückgegeben", fr="Rendue", es="Devuelta")
E("library.title_required", en="Please enter a title.", de="Bitte geben Sie einen Titel ein.",
  fr="Veuillez saisir un titre.", es="Introduzca un título.")
E("library.delete.confirm", en="Delete “%1” from the library?", de="„%1“ aus der Bibliothek löschen?",
  fr="Supprimer « %1 » de la bibliothèque ?", es="¿Eliminar «%1» de la biblioteca?", c="%1 = title")
E("more.rental_count", src="%n rental instrument(s)", en=["%n rental instrument", "%n rental instruments"],
  de=["%n Leihinstrument", "%n Leihinstrumente"], fr=["%n instrument de location", "%n instruments de location"],
  es=["%n instrumento de alquiler", "%n instrumentos de alquiler"])
E("instruments.rentals", en="Rental instruments", de="Leihinstrumente", fr="Instruments de location",
  es="Instrumentos de alquiler")
E("instruments.empty", en="No student has an instrument entered yet.",
  de="Bei noch keinem Schüler ist ein Instrument eingetragen.",
  fr="Aucun instrument n’est encore indiqué pour les élèves.", es="Aún no se ha indicado ningún instrumento.")
E("instruments.choices", en="Instruments to choose from", de="Instrumente zur Auswahl",
  fr="Instruments proposés", es="Instrumentos para elegir")
E("archive.empty", en="No archived students yet.", de="Noch keine archivierten Schüler.",
  fr="Aucun élève archivé pour l’instant.", es="Aún no hay alumnos archivados.")
E("archive.archived_on", en="Archived on %1", de="Archiviert am %1", fr="Archivé le %1", es="Archivado el %1",
  c="%1 = date")
E("archive.saved", en="Archive entry for %1 saved.", de="Archiveintrag für %1 gespeichert.",
  fr="Entrée d’archive enregistrée pour %1.", es="Entrada de archivo guardada para %1.", c="%1 = name")
E("archive.hint", en="Archive entries are read-only records: personal data, lessons, notes, pieces and activities "
     "at the time of archiving.",
  de="Archiveinträge sind feste Aufzeichnungen: persönliche Daten, Unterricht, Notizen, Werke und Aktivitäten zum "
     "Zeitpunkt der Archivierung.",
  fr="Les entrées d’archive sont des relevés figés : données personnelles, cours, notes, œuvres et activités au "
     "moment de l’archivage.",
  es="Las entradas de archivo son registros fijos: datos personales, clases, notas, obras y actividades en el "
     "momento de archivar.")
E("event.archive", en="Archive", de="Archivieren", fr="Archiver", es="Archivar")
E("events.archive.title", en="Archived events", de="Archivierte Veranstaltungen", fr="Événements archivés",
  es="Eventos archivados")
E("events.archive.empty", en="No archived events yet.", de="Noch keine archivierten Veranstaltungen.",
  fr="Aucun événement archivé pour l’instant.", es="Aún no hay eventos archivados.")
E("doc.timetable.title", en="Timetable", de="Stundenplan", fr="Emploi du temps", es="Horario")
E("doc.as_of", en="As of %1", de="Stand: %1", fr="État au %1", es="Actualizado el %1", c="%1 = date")
E("doc.total_years", src="%n year(s) in total", en=["%n year in total", "%n years in total"],
  de=["insgesamt %n Jahr", "insgesamt %n Jahre"], fr=["%n an au total", "%n ans au total"],
  es=["%n año en total", "%n años en total"])
E("doc.error.write", en="The document could not be created.", de="Das Dokument konnte nicht erstellt werden.",
  fr="Le document n’a pas pu être créé.", es="No se pudo crear el documento.")
E("doc.error.open", en="The document was saved but could not be opened: %1",
  de="Das Dokument wurde gespeichert, konnte aber nicht geöffnet werden: %1",
  fr="Le document a été enregistré mais n’a pas pu être ouvert : %1",
  es="El documento se guardó, pero no se pudo abrir: %1", c="%1 = file path")
E("doc.create", en="Create PDF", de="PDF erstellen", fr="Créer le PDF", es="Crear PDF")
E("doc.dayview.lines", en="lines", de="Zeilen", fr="lignes", es="líneas")
E("import.csv.rows", src="%n entries in the file", en=["%n entry in the file", "%n entries in the file"],
  de=["%n Eintrag in der Datei", "%n Einträge in der Datei"], fr=["%n entrée dans le fichier", "%n entrées dans le fichier"],
  es=["%n entrada en el archivo", "%n entradas en el archivo"])
E("import.csv.action", en="Import", de="Importieren", fr="Importer", es="Importar")
E("import.csv.hint", en="Each row with a name becomes a new student. Lessons can be added afterwards.",
  de="Jede Zeile mit einem Namen wird ein neuer Schüler. Den Unterricht können Sie danach zuordnen.",
  fr="Chaque ligne avec un nom devient un nouvel élève. Les cours peuvent être ajoutés ensuite.",
  es="Cada fila con un nombre se convierte en un alumno nuevo. Las clases se pueden asignar después.")
E("settings.this_device", en="On this device", de="Auf diesem Gerät", fr="Sur cet appareil", es="En este dispositivo")
E("settings.lesson_end.hint", en="A sound and a message while Qupil is open.",
  de="Ein Klang und eine Meldung, solange Qupil geöffnet ist.",
  fr="Un son et un message tant que Qupil est ouvert.", es="Un sonido y un mensaje mientras Qupil está abierto.")
E("app.build", en="Build %1", de="Build %1", fr="Build %1", es="Build %1", c="%1 = commit, e.g. 8828322")
E("app.build_copied", en="Copied: Qupil %1", de="Kopiert: Qupil %1", fr="Copié : Qupil %1", es="Copiado: Qupil %1",
  c="%1 = version and commit")
E("setup.sync.title", en="Data from another device", de="Daten von einem anderen Gerät",
  fr="Données d’un autre appareil", es="Datos de otro dispositivo")
E("setup.sync.text",
  en="Qupil already runs on another device? Connect this one to the same Nextcloud or pair it over Wi-Fi.",
  de="Qupil läuft schon auf einem anderen Gerät? Verbinden Sie dieses Gerät mit derselben Nextcloud oder koppeln "
     "Sie es per WLAN.",
  fr="Qupil fonctionne déjà sur un autre appareil ? Connectez celui-ci au même Nextcloud ou associez-le en Wi-Fi.",
  es="¿Qupil ya funciona en otro dispositivo? Conecte este al mismo Nextcloud o vincúlelo por Wi-Fi.")
E("setup.sync.action", en="Connect this device", de="Dieses Gerät verbinden", fr="Connecter cet appareil",
  es="Conectar este dispositivo")

# ---------------------------------------------------------------- settings
E("settings.title", old="Settings - Qupil", en="Settings", fr="Paramètres", es="Ajustes",
  merged=["&Settings"])
E("settings.page.general", old="General", en="General", fr="Général", es="General")
E("settings.language", old="Language:", en="Language:", fr="Langue :", es="Idioma:")
E("settings.appearance", en="Appearance", de="Darstellung", fr="Apparence", es="Apariencia")
E("appearance.system", en="System", de="System", fr="Système", es="Sistema",
  c="Follow the light/dark setting of the operating system.")
E("appearance.light", en="Light", de="Hell", fr="Clair", es="Claro")
E("appearance.dark", en="Dark", de="Dunkel", fr="Sombre", es="Oscuro")
E("settings.birthday_reminder", old="Activate Birthday Reminder", en="Enable birthday reminders",
  fr="Activer les rappels d’anniversaire", es="Activar recordatorios de cumpleaños")
E("settings.lesson_end.enable", old="Notification at the end of lessons",
  en="Alert before the end of a lesson", fr="Alerte avant la fin du cours",
  es="Aviso antes del final de la clase")
E("settings.lesson_end.before", old="before the end of the lesson", en="before the end of the lesson",
  fr="avant la fin du cours", es="antes del final de la clase",
  c="Follows a number field with minutes: [3 min] [before the end of the lesson].")
E("settings.play_sound", old="Play Sound:", en="Play sound:", fr="Son à jouer :", es="Sonido:")
E("settings.volume", old="Volume:", en="Volume:", fr="Volume :", es="Volumen:")
E("settings.reminder_sound", old="Sound for the reminder tool", en="Reminder sound",
  fr="Son des rappels", es="Sonido de los recordatorios")
E("settings.save_for_all", old="Save notes / music pieces together for all students of the lesson",
  en="Save notes / pieces together for all students of a lesson",
  fr="Enregistrer les notes / morceaux pour tous les élèves du cours",
  es="Guardar notas / obras para todos los alumnos de la clase")
E("settings.recital_check", old="Concert Interval Check (Concert Candidates) considered:",
  en="The recital interval check (recital candidates) takes into account:",
  fr="Le contrôle de l’intervalle entre auditions (candidats) prend en compte :",
  es="La comprobación del intervalo entre audiciones (candidatos) tiene en cuenta:")
E("settings.recital_check.solo", old="Only Solo Recitals", en="Solo performances only",
  fr="Prestations en solo uniquement", es="Únicamente actuaciones solistas")
E("settings.recital_check.solo_ensemble", old="Solo Recitals and Ensemble Participation",
  en="Solo performances and ensemble participation",
  fr="Prestations en solo et participation à un ensemble",
  es="Actuaciones solistas y participación en un conjunto")
E("settings.colors", old="Colors", en="Colors", fr="Couleurs", es="Colores")
E("settings.colors.text", old="Text", en="Text", fr="Texte", es="Texto")
E("settings.colors.background", old="Background", en="Background", fr="Arrière-plan", es="Fondo")
E("settings.colors.hint",
  en="The main colour is used for buttons, selections and headings; the highlight marks what is happening now.",
  de="Die Hauptfarbe nutzen Schaltflächen, Auswahl und Überschriften; die Hervorhebung zeigt, was gerade läuft.",
  fr="La couleur principale sert aux boutons, aux sélections et aux titres ; la mise en évidence montre ce qui "
     "se passe maintenant.",
  es="El color principal se usa en botones, selecciones y títulos; el resaltado muestra lo que ocurre ahora.")
E("settings.colors.accent", en="Main colour", de="Hauptfarbe", fr="Couleur principale", es="Color principal")
E("settings.colors.highlight", en="Highlight", de="Hervorhebung", fr="Mise en évidence", es="Resaltado")
E("settings.colors.mark", en="Marking in notes", de="Markierung in Notizen", fr="Marquage dans les notes",
  es="Marcado en las notas", c="Colour of marked text in lesson notes.")
E("settings.colors.custom", en="Your own colour", de="Eigene Farbe", fr="Couleur personnalisée", es="Color propio")
E("settings.colors.reset", en="Default colours", de="Standardfarben", fr="Couleurs par défaut",
  es="Colores predeterminados")
E("settings.limit_notes", old="Limit the number of displayed lesson notes    ",
  en="Limit the number of lesson notes shown", fr="Limiter le nombre de notes de cours affichées",
  es="Limitar el número de notas de clase mostradas")
E("settings.limit_notes.count", old="Number of displayed lesson notes:",
  en="Number of lesson notes shown:", fr="Nombre de notes de cours affichées :",
  es="Número de notas de clase mostradas:")
E("settings.limit_pieces", old="Limit the number of displayed music pieces   ",
  en="Limit the number of pieces shown", fr="Limiter le nombre de morceaux affichés",
  es="Limitar el número de obras mostradas")
E("settings.limit_pieces.count", old="Number of displayed music pieces:", en="Number of pieces shown:",
  fr="Nombre de morceaux affichés :", es="Número de obras mostradas:")
E("settings.locations", old="Lesson Locations", en="Lesson locations", fr="Lieux de cours",
  es="Lugares de clase")
E("settings.sizes", old="Sizes", en="Sizes", fr="Tailles", es="Tamaños")
E("settings.genres", old="Music Styles", en="Styles", fr="Styles", es="Estilos")
E("settings.genre_label", old="Style:", en="Style:", fr="Style :", es="Estilo:")
E("settings.event_duration", old="Calculate total playing time of events",
  en="Calculate the total duration of events", fr="Calculer la durée totale des événements",
  es="Calcular la duración total de los eventos")
E("settings.event.moderation", old="Total time for moderation:",
  en="Total time for introduction and closing remarks:", fr="Durée totale de présentation :",
  es="Tiempo total de presentación:")
E("settings.event.changeover", old="Break between each music piece:", en="Changeover time between pieces:",
  fr="Temps de changement entre les morceaux :", es="Tiempo de cambio entre obras:")
E("settings.select_sound", old="Select Sound File", en="Select sound file", fr="Choisir un fichier son",
  es="Seleccionar archivo de sonido")
E("settings.filter_sound", old="Sound Files (*.wav *.ogg *.mp3)",
  en="Sound files (*.wav *.mp3 *.m4a)", de="Klangdateien (*.wav *.mp3 *.m4a)",
  fr="Fichiers audio (*.wav *.mp3 *.m4a)", es="Archivos de audio (*.wav *.mp3 *.m4a)",
  c="File dialog filter. Keep the pattern in parentheses unchanged.",
  note="Dateimuster ohne OGG (iOS spielt OGG nicht zuverlässig), dafür M4A")
E("language.system", en="System language", de="Systemsprache", fr="Langue du système",
  es="Idioma del sistema")
E("language.en", old="English", en="English", fr="Anglais", es="Inglés")
E("language.de", old="German", en="German", fr="Allemand", es="Alemán")
E("language.fr", en="French", de="Französisch", fr="Français", es="Francés")
E("language.es", en="Spanish", de="Spanisch", fr="Espagnol", es="Español")

# ---------------------------------------------------------------- "More" page and notifications (new)
E("more.manage", en="Management", de="Verwaltung", fr="Gestion", es="Gestión")
E("more.reports", en="Reports", de="Auswertungen", fr="Rapports", es="Informes")
E("more.documents", en="Documents (share or print as PDF)", de="Dokumente (PDF teilen oder drucken)",
  fr="Documents (partager ou imprimer en PDF)", es="Documentos (compartir o imprimir en PDF)")
E("more.data", en="Data", de="Daten", fr="Données", es="Datos")
E("more.lent_count", src="%n on loan",
  en=["%n on loan", "%n on loan"], de=["%n verliehen", "%n verliehen"],
  fr=["%n prêtée", "%n prêtées"], es=["%n prestada", "%n prestadas"],
  c="Number of sheet music items currently lent out.")
E("more.active_count", src="%n active",
  en=["%n active", "%n active"], de=["%n aktiv", "%n aktiv"],
  fr=["%n actif", "%n actifs"], es=["%n activo", "%n activos"],
  c="Number of active reminders.")
E("notify.lesson_end", src="Lesson ends in %n minute(s)",
  en=["Lesson ends in %n minute", "Lesson ends in %n minutes"],
  de=["Unterricht endet in %n Minute", "Unterricht endet in %n Minuten"],
  fr=["Le cours se termine dans %n minute", "Le cours se termine dans %n minutes"],
  es=["La clase termina en %n minuto", "La clase termina en %n minutos"],
  c="System notification before the end of a lesson.")
E("notify.permission",
  en="Allow notifications so that Qupil can remind you even when the app is closed.",
  de="Erlauben Sie Mitteilungen, damit Qupil Sie auch bei geschlossener App erinnern kann.",
  fr="Autorisez les notifications pour que Qupil puisse vous prévenir même lorsque l’application est fermée.",
  es="Permita las notificaciones para que Qupil pueda avisarle aunque la app esté cerrada.")

E("notify.channel.lesson_end", en="End of lesson", de="Unterrichtsende", fr="Fin du cours", es="Fin de la clase",
  c="Name of the notification category in the system settings (Android).")
E("notify.denied",
  en="Notifications for Qupil are turned off, so there is no alert while Qupil is closed.",
  de="Mitteilungen für Qupil sind ausgeschaltet. Bei geschlossener App gibt es deshalb keine Erinnerung.",
  fr="Les notifications de Qupil sont désactivées : aucune alerte lorsque Qupil est fermé.",
  es="Las notificaciones de Qupil están desactivadas, así que no hay aviso con Qupil cerrado.")
E("notify.exact_alarms",
  en="For the alert to come on time, allow Qupil to set alarms and reminders.",
  de="Damit die Erinnerung pünktlich kommt, erlauben Sie Qupil „Wecker und Erinnerungen“.",
  fr="Pour que l’alerte arrive à l’heure, autorisez Qupil à définir des alarmes et des rappels.",
  es="Para que el aviso llegue puntual, permita a Qupil programar alarmas y recordatorios.",
  c="Android 12+: the special permission 'Alarms & reminders'.")
E("notify.allow", en="Allow", de="Erlauben", fr="Autoriser", es="Permitir")
E("notify.open_settings", en="Open settings", de="Einstellungen öffnen", fr="Ouvrir les réglages",
  es="Abrir ajustes")
E("settings.lesson_end.hint_mobile", en="A notification with sound, also when Qupil is closed.",
  de="Eine Mitteilung mit Klang, auch wenn Qupil geschlossen ist.",
  fr="Une notification avec un son, même lorsque Qupil est fermé.",
  es="Una notificación con sonido, también con Qupil cerrado.")
E("settings.sound.default_name", en="Ahem (default)", de="Ähem (Standard)", fr="Hum (par défaut)",
  es="Ejem (predeterminado)", c="The sound of Qupil 1.x: someone clearing their throat.")
E("settings.sound.reminder_default", en="Ping (default)", de="Pling (Standard)", fr="Ding (par défaut)",
  es="Ding (predeterminado)", c="The reminder sound of Qupil 1.x: a short bright ping.")
E("settings.reminder_sound.hint",
  en="Plays when a reminder with sound appears: when Qupil starts or when a lesson starts.",
  de="Erklingt, wenn eine Erinnerung mit Klang erscheint: beim Start von Qupil oder zu Beginn eines Unterrichts.",
  fr="Retentit quand un rappel avec son apparaît : au démarrage de Qupil ou au début d’un cours.",
  es="Suena cuando aparece un recordatorio con sonido: al iniciar Qupil o al comenzar una clase.")
E("settings.sound.use_default", en="Default sound", de="Standardklang", fr="Son par défaut",
  es="Sonido predeterminado", c="Button: go back to the sound that comes with Qupil.")
E("settings.sound.test", en="Listen", de="Anhören", fr="Écouter", es="Escuchar")
E("settings.sound.hint", en="Your own sound: WAV, MP3 or M4A. Longer sounds stop after 10 seconds.",
  de="Eigener Klang: WAV, MP3 oder M4A. Längere Klänge enden nach 10 Sekunden.",
  fr="Votre propre son : WAV, MP3 ou M4A. Les sons plus longs s’arrêtent après 10 secondes.",
  es="Sonido propio: WAV, MP3 o M4A. Los sonidos más largos se cortan a los 10 segundos.")
E("settings.sound.error", en="This file cannot be played. Please choose a WAV, MP3 or M4A file.",
  de="Diese Datei lässt sich nicht abspielen. Bitte wählen Sie eine WAV-, MP3- oder M4A-Datei.",
  fr="Impossible de lire ce fichier. Veuillez choisir un fichier WAV, MP3 ou M4A.",
  es="No se puede reproducir este archivo. Elija un archivo WAV, MP3 o M4A.")

# ---------------------------------------------------------------- default list entries
E("defaults.genre.song", old="Tune", pick="Lied", en="Song", fr="Chanson", es="Canción",
  c="Default music style created on first start; users can rename it.")
E("defaults.genre.baroque", old="Baroque", en="Baroque", fr="Baroque", es="Barroco")
E("defaults.genre.classical", old="Classical", en="Classical", fr="Classique", es="Clásico")
E("defaults.genre.romantic", old="Romantic", en="Romantic", fr="Romantique", es="Romántico")
E("defaults.genre.modern", old="Modernism", en="Modern", fr="Moderne", es="Moderno")
E("defaults.genre.new_music", old="New music", en="Contemporary", fr="Musique contemporaine",
  es="Música contemporánea")
E("defaults.genre.musical", old="Musical", en="Musical", fr="Comédie musicale", es="Musical")
E("defaults.genre.jazz", old="Jazz", en="Jazz", fr="Jazz", es="Jazz")
E("defaults.genre.rock", old="Rock", en="Rock", fr="Rock", es="Rock")
E("defaults.genre.pop", old="Pop", en="Pop", fr="Pop", es="Pop")
E("defaults.genre.irish_folk", old="Irish Folk", en="Irish folk", fr="Musique irlandaise",
  es="Folk irlandés")
E("defaults.location.room20", old="Room 20", en="Room 20", fr="Salle 20", es="Aula 20",
  c="Default lesson location created on first start; users can rename it.")
E("defaults.location.room16", old="Room 16", en="Room 16", fr="Salle 16", es="Aula 16")
E("defaults.location.auditorium", old="Auditorium", en="Auditorium", fr="Auditorium", es="Auditorio")
E("defaults.instrument.violin", old="Violin", en="Violin", fr="Violon", es="Violín",
  c="Default instrument created on first start; users can rename it.")
E("defaults.instrument.viola", old="Viola", en="Viola", fr="Alto", es="Viola")
E("defaults.instrument.cello", old="Cello", en="Cello", fr="Violoncelle", es="Violonchelo")
E("defaults.instrument.double_bass", old="Double Bass", en="Double bass", fr="Contrebasse",
  es="Contrabajo")

# ---------------------------------------------------------------- about
E("about.title", old="About Qupil ", en="About Qupil", fr="À propos de Qupil", es="Acerca de Qupil")
E("about.tab.about", old="About", en="About", fr="À propos", es="Acerca de")
E("about.tab.project", old="Project", en="Project", fr="Projet", es="Proyecto")
E("about.tab.translation", old="Translation", en="Translation", fr="Traduction", es="Traducción")
E("about.tab.license", old="License", en="License", fr="Licence", es="Licencia")
E("about.version", en="Version %1", de="Version %1", fr="Version %1", es="Versión %1", c="%1 = version")
E("about.website", en="Website", de="Website", fr="Site web", es="Sitio web")
E("about.author", en="Author", de="Autor", fr="Auteur", es="Autor")
E("about.source", en="Source code", de="Quellcode", fr="Code source", es="Código fuente")
E("about.license.show", en="License text", de="Lizenztext", fr="Texte de la licence", es="Texto de la licencia")
E("about.license.summary",
  en="Qupil is free software: you can redistribute it and/or modify it under the terms of the GNU General Public "
     "License, version 3 or (at your option) any later version. An additional permission allows distribution "
     "through app stores as long as the complete source code stays freely available. Qupil comes without any "
     "warranty.",
  de="Qupil ist freie Software: Sie können es unter den Bedingungen der GNU General Public License, Version 3 oder "
     "(nach Ihrer Wahl) jeder späteren Version, weitergeben und verändern. Eine zusätzliche Erlaubnis gestattet die "
     "Verbreitung über App-Stores, solange der vollständige Quellcode frei verfügbar bleibt. Qupil wird ohne jede "
     "Gewährleistung bereitgestellt.",
  fr="Qupil est un logiciel libre : vous pouvez le redistribuer et/ou le modifier selon les termes de la GNU "
     "General Public License, version 3 ou (à votre choix) toute version ultérieure. Une autorisation "
     "supplémentaire permet la distribution via les magasins d’applications, à condition que le code source "
     "complet reste librement disponible. Qupil est fourni sans aucune garantie.",
  es="Qupil es software libre: puede redistribuirlo y/o modificarlo según los términos de la GNU General Public "
     "License, versión 3 o (a su elección) cualquier versión posterior. Un permiso adicional permite distribuirlo "
     "a través de tiendas de aplicaciones, siempre que el código fuente completo siga disponible libremente. "
     "Qupil se ofrece sin ninguna garantía.")
E("about.components",
  en="Qupil uses Qt (GNU LGPL 3.0), the Figtree font (SIL Open Font License 1.1) and Monocypher (CC0 / BSD 2-Clause).",
  de="Qupil verwendet Qt (GNU LGPL 3.0), die Schrift Figtree (SIL Open Font License 1.1) und Monocypher "
     "(CC0 / BSD 2-Clause).",
  fr="Qupil utilise Qt (GNU LGPL 3.0), la police Figtree (SIL Open Font License 1.1) et Monocypher "
     "(CC0 / BSD 2-Clause).",
  es="Qupil utiliza Qt (GNU LGPL 3.0), la fuente Figtree (SIL Open Font License 1.1) y Monocypher "
     "(CC0 / BSD 2-Clause).")
E("about.features", old="__HTML_FEATURES__",
  de="- Verwaltung von Unterrichts- und Schülerdaten\n- Veranstaltungsmanager\n- Metronom, Stimmgerät\n- Notenbibliothek",
  en="- Student and lesson management\n- Event manager\n- Metronome, tuner\n- Sheet music library",
  fr="- Gestion des élèves et des cours\n- Gestionnaire d’événements\n- Métronome, accordeur\n- Bibliothèque de partitions",
  es="- Gestión de alumnos y clases\n- Gestor de eventos\n- Metrónomo, afinador\n- Biblioteca de partituras",
  note="Aus dem HTML-Text des Designers extrahiert")
E("about.project", old="__HTML_PROJECT__", de="Projekt-Website: %1\nAutor: %2",
  en="Project website: %1\nAuthor: %2", fr="Site du projet : %1\nAuteur : %2",
  es="Sitio web del proyecto: %1\nAutor: %2", c="%1 = website URL, %2 = author name and e-mail",
  note="Aus dem HTML-Text extrahiert, Website und Autor als Platzhalter")
E("about.translation_help", old="__HTML_TRANSLATION__",
  de="Wenn Sie bei der Übersetzung von Qupil helfen möchten, kontaktieren Sie bitte den Autor via %1",
  en="If you would like to help translate Qupil, please contact the author at %1.",
  fr="Si vous souhaitez aider à traduire Qupil, contactez l’auteur à l’adresse %1.",
  es="Si desea ayudar a traducir Qupil, póngase en contacto con el autor en %1.",
  c="%1 = contact e-mail address",
  note="Aus dem HTML-Text extrahiert, E-Mail als Platzhalter; die Liste der Übersetzer erzeugt Qupil 2 selbst")
