# -*- coding: utf-8 -*-
"""Texts of the student app: ID -> English, German, French, Spanish.

The app talks to the student with "du" / "tu" / "tú". python3 -I strings.py writes
strings.json, which the app reads at start (src/Translator.cpp). Plural texts are
a dict {"one": …, "other": …}; %n is the number.
"""
import json
import os

S = {}


def E(id, en, de, fr, es):
    assert id not in S, id
    S[id] = {"en": en, "de": de, "fr": fr, "es": es}


E("s.app.name", "Qupil – Music Practice", "Qupil – Musik üben", "Qupil – Music Practice", "Qupil – Music Practice")
E("s.app.section", "Tasks", "Aufgaben", "Devoirs", "Tareas")
E("s.back", "Back", "Zurück", "Retour", "Atrás")

# Date patterns (Qt date format)
E("s.fmt.short", "d MMM", "d.M.", "d MMM", "d MMM")
E("s.fmt.weekday_short", "ddd d MMM", "ddd, d.M.", "ddd d MMM", "ddd d MMM")
E("s.fmt.long", "dddd d MMMM", "dddd, d. MMMM", "dddd d MMMM", "dddd d 'de' MMMM")
E("s.fmt.month_short", "MMM", "MMM", "MMM", "MMM")
E("s.fmt.day_number", "d", "d", "d", "d")
E("s.when.today", "today", "heute", "aujourd'hui", "hoy")
E("s.when.tomorrow", "tomorrow", "morgen", "demain", "mañana")
E("s.when.in_days", {"one": "in %n day", "other": "in %n days"}, {"one": "in %n Tag", "other": "in %n Tagen"},
  {"one": "dans %n jour", "other": "dans %n jours"}, {"one": "en %n día", "other": "en %n días"})
E("s.when.today_at", "today, %1", "heute, %1", "aujourd'hui, %1", "hoy, %1")

# Welcome and coupling
E("s.welcome.title", "Welcome to Qupil Practice", "Willkommen bei Qupil Üben", "Bienvenue dans Qupil Practice",
  "Te damos la bienvenida a Qupil Practice")
E("s.welcome.text",
  "Here you'll find what your teacher gives you to practise – often with tips. If you like, you can let them know how it went.",
  "Hier findest du, was deine Lehrerin oder dein Lehrer dir zum Üben mitgibt – oft mit Tipps. Wenn du magst, kannst du kurz zurückmelden, wie es lief.",
  "Ici, tu trouves ce que ton professeur te donne à travailler – souvent avec des conseils. Si tu veux, tu peux lui dire comment ça s'est passé.",
  "Aquí encuentras lo que tu profesor o profesora te da para practicar, a menudo con consejos. Si quieres, puedes contarle qué tal te fue.")
E("s.welcome.privacy",
  "No account, no e-mail address. What you write goes encrypted to your teacher only.",
  "Kein Konto, keine E-Mail-Adresse. Was du schreibst, geht verschlüsselt nur an deine Lehrkraft.",
  "Pas de compte, pas d'adresse e-mail. Ce que tu écris va, chiffré, uniquement à ton professeur.",
  "Sin cuenta ni correo electrónico. Lo que escribes llega cifrado solo a tu profesor o profesora.")
E("s.add.scan.title", "Scan the QR code", "QR-Code scannen", "Scanner le code QR", "Escanear el código QR")
E("s.add.scan.text",
  "Your teacher shows you a code in Qupil. Point your phone's camera at it – your tasks then show up here by themselves.",
  "Deine Lehrkraft zeigt dir in Qupil einen Code. Halte die Kamera deines Handys darauf – deine Aufgaben erscheinen dann von selbst hier.",
  "Ton professeur te montre un code dans Qupil. Vise-le avec l'appareil photo de ton téléphone – tes devoirs s'affichent alors ici tout seuls.",
  "Tu profesor te muestra un código en Qupil. Apunta la cámara del móvil hacia él y tus tareas aparecen aquí solas.")
# With the scanner in the app (phones and tablets)
E("s.add.scan.text_app",
  "Your teacher shows you a code in Qupil. Scan it here – your tasks then show up by themselves.",
  "Deine Lehrkraft zeigt dir in Qupil einen Code. Scanne ihn hier – deine Aufgaben erscheinen dann von selbst.",
  "Ton professeur te montre un code dans Qupil. Scanne-le ici – tes devoirs s'affichent alors tout seuls.",
  "Tu profesor te muestra un código en Qupil. Escanéalo aquí y tus tareas aparecen solas.")
E("s.add.scan.button", "Open camera", "Kamera öffnen", "Ouvrir l'appareil photo", "Abrir la cámara")
E("s.add.scan.prompt", "Point the camera at the code in your teacher's Qupil.",
  "Halte die Kamera auf den Code im Qupil deiner Lehrkraft.",
  "Vise le code dans le Qupil de ton professeur.", "Apunta la cámara al código en el Qupil de tu profesor.")
E("s.add.scan.cancel", "Cancel", "Abbrechen", "Annuler", "Cancelar")
E("s.add.scan.invalid", "This is not a Qupil code. Ask your teacher to show you the code again.",
  "Das ist kein Qupil-Code. Lass dir den Code noch einmal von deiner Lehrkraft zeigen.",
  "Ce n'est pas un code Qupil. Demande à ton professeur de te remontrer le code.",
  "Esto no es un código de Qupil. Pide a tu profesor que te lo muestre de nuevo.")
E("s.add.scan.denied",
  "The camera is not allowed for this app. You can allow it in your phone's settings – or paste the link below.",
  "Die Kamera ist für diese App nicht erlaubt. Du kannst sie in den Einstellungen deines Handys erlauben – oder unten den Link einfügen.",
  "L'appareil photo n'est pas autorisé pour cette application. Tu peux l'autoriser dans les réglages de ton téléphone – ou coller le lien ci-dessous.",
  "La cámara no está permitida para esta aplicación. Puedes permitirla en los ajustes del móvil o pegar el enlace abajo.")
E("s.add.link.title", "Got a link?", "Link bekommen?", "Tu as reçu un lien ?", "¿Te enviaron un enlace?")
E("s.add.link.text", "If your teacher sent you the link, paste it here.",
  "Wenn deine Lehrkraft dir den Link geschickt hat, füge ihn hier ein.",
  "Si ton professeur t'a envoyé le lien, colle-le ici.",
  "Si tu profesor te envió el enlace, pégalo aquí.")
E("s.add.paste", "Paste", "Einfügen", "Coller", "Pegar")
E("s.add.open", "Open", "Öffnen", "Ouvrir", "Abrir")
E("s.add.invalid", "This is not a Qupil link. Ask your teacher for a new one.",
  "Das ist kein Qupil-Link. Frag deine Lehrkraft nach einem neuen.",
  "Ce n'est pas un lien Qupil. Demande-en un nouveau à ton professeur.",
  "Esto no es un enlace de Qupil. Pide uno nuevo a tu profesor.")
E("s.add.clipboard_empty", "There is no link on the clipboard.", "In der Zwischenablage ist kein Link.",
  "Aucun lien dans le presse-papiers.", "No hay ningún enlace en el portapapeles.")
E("s.add.demo", "See an example", "Beispiel ansehen", "Voir un exemple", "Ver un ejemplo")
E("s.coupled", "Teacher added", "Lehrkraft hinzugefügt", "Professeur ajouté", "Profesor añadido")

# Booklet
E("s.demo.banner", "Example with made-up data – nothing is sent.",
  "Beispiel mit ausgedachten Daten – es wird nichts gesendet.",
  "Exemple avec des données inventées – rien n'est envoyé.",
  "Ejemplo con datos inventados: no se envía nada.")
E("s.demo.end", "Close", "Beenden", "Fermer", "Cerrar")
E("s.home.refresh", "Update", "Aktualisieren", "Actualiser", "Actualizar")
E("s.home.loading", "Loading your tasks …", "Aufgaben werden geladen …", "Chargement des devoirs …", "Cargando las tareas …")
E("s.home.not_loaded", "Not loaded yet. Pull down to try again.", "Noch nicht geladen. Zum Aktualisieren nach unten ziehen.",
  "Pas encore chargé. Tire vers le bas pour réessayer.", "Aún no se ha cargado. Desliza hacia abajo para reintentar.")
E("s.home.updated", "Updated %1", "Aktualisiert %1", "Mis à jour %1", "Actualizado %1")
E("s.error.offline", "No connection – you see the last state.", "Keine Verbindung – du siehst den letzten Stand.",
  "Pas de connexion – tu vois le dernier état.", "Sin conexión: ves el último estado.")
E("s.error.gone", "%1 has ended the connection. You can remove the entry in the settings.",
  "%1 hat die Verbindung beendet. Du kannst den Eintrag in den Einstellungen entfernen.",
  "%1 a mis fin à la connexion. Tu peux retirer l'entrée dans les réglages.",
  "%1 ha terminado la conexión. Puedes quitar la entrada en los ajustes.")
E("s.error.key", "Your tasks cannot be opened. Ask your teacher for a new code.",
  "Die Aufgaben lassen sich nicht öffnen. Frag deine Lehrkraft nach einem neuen Code.",
  "Impossible d'ouvrir les devoirs. Demande un nouveau code à ton professeur.",
  "No se pueden abrir las tareas. Pide un código nuevo a tu profesor.")
E("s.error.newer", "Your teacher uses a newer Qupil. Please update this app.",
  "Deine Lehrkraft nutzt ein neueres Qupil. Bitte aktualisiere diese App.",
  "Ton professeur utilise un Qupil plus récent. Mets cette application à jour.",
  "Tu profesor usa un Qupil más reciente. Actualiza esta aplicación.")
E("s.teacher_fallback", "your teacher", "deine Lehrkraft", "ton professeur", "tu profesor")
E("s.next.label", "Next lesson", "Nächste Stunde", "Prochain cours", "Próxima clase")
E("s.next.none", "No lesson planned yet", "Noch keine Stunde geplant", "Aucun cours prévu", "Ninguna clase prevista")
E("s.next.cancelled", "cancelled", "fällt aus", "annulé", "cancelada")
E("s.tasks.from", "Tasks from %1", "Aufgaben vom %1", "Devoirs du %1", "Tareas del %1")
E("s.tasks.none", "No tasks right now.", "Gerade keine Aufgaben.", "Pas de devoirs pour l'instant.", "Ahora no hay tareas.")
E("s.task.hints", "%1 tips", "%1 Hinweise", "%1 conseils", "%1 consejos")
E("s.task.open_since", "open since %1", "offen seit %1", "ouvert depuis le %1", "pendiente desde %1")
E("s.fb.practiced", "Practised", "Geübt", "Travaillé", "Practicado")
E("s.fb.practiced.3", "a lot", "viel", "beaucoup", "mucho")
E("s.fb.practiced.2", "some", "etwas", "un peu", "algo")
E("s.fb.practiced.1", "hardly", "kaum", "à peine", "poco")
E("s.fb.works", "Works", "Klappt", "Ça marche", "Sale")
E("s.fb.works.3", "well", "gut", "bien", "bien")
E("s.fb.works.2", "so-so", "geht so", "moyen", "más o menos")
E("s.fb.works.1", "not yet", "noch nicht", "pas encore", "aún no")
# The teacher types the name freely ("Herr Hammer"); German cannot decline it, so no "an %1" there
E("s.send.sent", "Sent to %1 · %2", "Geschickt · %1 · %2", "Envoyé à %1 · %2", "Enviado a %1 · %2")
E("s.send.sent_plain", "Sent to your teacher · %1", "An deine Lehrkraft geschickt · %1", "Envoyé à ton professeur · %1",
  "Enviado a tu profesor · %1")
E("s.send.pending", "Will be sent …", "Wird gesendet …", "Envoi en cours …", "Se enviará …")
E("s.question.title", "Question to %1", "Deine Frage · %1", "Question à %1", "Pregunta para %1")
E("s.question.title_plain", "Question to your teacher", "Frage an deine Lehrkraft", "Question à ton professeur",
  "Pregunta para tu profesor")
E("s.question.placeholder", "Something unclear? Ask here …", "Etwas unklar? Frag hier …",
  "Quelque chose n'est pas clair ? Demande ici …", "¿Algo no está claro? Pregunta aquí …")
E("s.question.hint", "Your teacher reads it before the next lesson.", "Deine Lehrkraft liest das vor der nächsten Stunde.",
  "Ton professeur le lira avant le prochain cours.", "Tu profesor lo leerá antes de la próxima clase.")
E("s.question.demo", "In the example nothing is sent.", "Im Beispiel wird nichts gesendet.",
  "Dans l'exemple, rien n'est envoyé.", "En el ejemplo no se envía nada.")
E("s.question.send", "Send", "Senden", "Envoyer", "Enviar")
E("s.pieces.title", "Pieces", "Stücke", "Morceaux", "Obras")
E("s.pieces.ready", "ready", "sitzt", "prêt", "lista")
E("s.events.title", "Dates", "Termine", "Dates", "Fechas")
E("s.events.you_play", "You play: %1", "Du spielst: %1", "Tu joues : %1", "Tocas: %1")

# Settings
E("s.settings.title", "Settings", "Einstellungen", "Réglages", "Ajustes")
E("s.settings.teachers", "Teachers", "Lehrkräfte", "Professeurs", "Profesores")
E("s.settings.unknown_teacher", "Teacher", "Lehrkraft", "Professeur", "Profesor")
E("s.settings.demo", "Example", "Beispiel", "Exemple", "Ejemplo")
E("s.settings.remove", "Remove", "Entfernen", "Retirer", "Quitar")
E("s.settings.remove_confirm", "Really remove", "Wirklich entfernen", "Vraiment retirer", "Quitar de verdad")
E("s.settings.remove_hint",
  "This removes the connection to this teacher from this phone only. Your teacher is told that it was removed from one device, and can give you a new code at any time.",
  "Das entfernt die Verbindung zu dieser Lehrkraft nur von diesem Handy. Deine Lehrkraft erfährt, dass sie auf einem Gerät entfernt wurde, und kann dir jederzeit einen neuen Code geben.",
  "La connexion à ce professeur n'est retirée que de ce téléphone. Ton professeur apprend qu'elle a été retirée d'un appareil et peut te donner un nouveau code à tout moment.",
  "Solo se quita la conexión con este profesor de este móvil. Tu profesor se entera de que se quitó de un dispositivo y puede darte un código nuevo cuando quieras.")
E("s.settings.add", "Add another teacher", "Weitere Lehrkraft hinzufügen", "Ajouter un autre professeur", "Añadir otro profesor")
E("s.settings.privacy", "Privacy", "Datenschutz", "Confidentialité", "Privacidad")
E("s.settings.version_copied", "Copied: %1", "Kopiert: %1", "Copié : %1", "Copiado: %1")
E("s.settings.privacy_text",
  "Your tasks lie encrypted in your teacher's cloud. Only this app and your teacher's Qupil can read them. "
  "What you send back (practised, works, questions) goes encrypted to your teacher only. "
  "Your teacher also sees on how many devices the tasks are set up – not which ones, and not when you use the app. The app has no account, "
  "no advertising and no tracking.",
  "Deine Aufgaben liegen verschlüsselt in der Cloud deiner Lehrkraft. Lesen können sie nur diese App und das Qupil deiner Lehrkraft. "
  "Was du zurückschickst (geübt, klappt, Fragen), geht verschlüsselt nur an deine Lehrkraft. "
  "Deine Lehrkraft sieht außerdem, auf wie vielen Geräten ihre Aufgaben eingerichtet sind – nicht, auf welchen, und nicht, wann du die App benutzt. Die App hat kein Konto, "
  "keine Werbung und kein Tracking.",
  "Tes devoirs sont stockés chiffrés dans le cloud de ton professeur. Seuls cette application et le Qupil de ton professeur peuvent les lire. "
  "Ce que tu renvoies (travaillé, ça marche, questions) va, chiffré, uniquement à ton professeur. "
  "Ton professeur voit aussi sur combien d'appareils les devoirs sont installés – pas lesquels, ni quand tu utilises l'application. L'application n'a ni compte, "
  "ni publicité, ni pistage.",
  "Tus tareas están cifradas en la nube de tu profesor. Solo esta aplicación y el Qupil de tu profesor pueden leerlas. "
  "Lo que envías (practicado, sale, preguntas) llega cifrado solo a tu profesor. "
  "Tu profesor también ve en cuántos dispositivos están las tareas, no en cuáles ni cuándo usas la aplicación. La aplicación no tiene cuenta, "
  "ni publicidad, ni rastreo.")

if __name__ == "__main__":
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "strings.json")
    with open(out, "w", encoding="utf-8") as f:
        json.dump(S, f, ensure_ascii=False, indent=1, sort_keys=True)
        f.write("\n")
    print(len(S), "texts ->", out)
