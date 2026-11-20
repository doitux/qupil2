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


E("s.app.name", "Qupil for students", "Qupil für Schüler", "Qupil pour élèves", "Qupil para alumnos")
E("s.app.section", "Practice book", "Übeheft", "Cahier", "Cuaderno")
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
E("s.welcome.title", "Your practice book", "Dein Übeheft", "Ton cahier d'exercices", "Tu cuaderno de práctica")
E("s.welcome.text",
  "Your teacher writes down here what to practise until the next lesson – with tips if you like. You tell them how it went.",
  "Hier schreibt deine Lehrerin oder dein Lehrer auf, was du bis zur nächsten Stunde übst – auf Wunsch mit Tipps. Du sagst kurz, wie es lief.",
  "Ici, ton professeur note ce que tu travailles jusqu'au prochain cours – avec des conseils si tu veux. Tu lui dis comment ça s'est passé.",
  "Aquí tu profesor o profesora apunta lo que practicas hasta la próxima clase, con consejos si quieres. Tú le cuentas qué tal te fue.")
E("s.welcome.privacy",
  "No account, no e-mail address. What you write goes encrypted to your teacher only.",
  "Kein Konto, keine E-Mail-Adresse. Was du schreibst, geht verschlüsselt nur an deine Lehrkraft.",
  "Pas de compte, pas d'adresse e-mail. Ce que tu écris va, chiffré, uniquement à ton professeur.",
  "Sin cuenta ni correo electrónico. Lo que escribes llega cifrado solo a tu profesor o profesora.")
E("s.add.scan.title", "Scan the QR code", "QR-Code scannen", "Scanner le code QR", "Escanear el código QR")
E("s.add.scan.text",
  "Your teacher shows you a code in Qupil. Point your phone's camera at it – the practice book opens here by itself.",
  "Deine Lehrkraft zeigt dir in Qupil einen Code. Halte die Kamera deines Handys darauf – das Übeheft öffnet sich dann von selbst hier.",
  "Ton professeur te montre un code dans Qupil. Vise-le avec l'appareil photo de ton téléphone – le cahier s'ouvre ici tout seul.",
  "Tu profesor te muestra un código en Qupil. Apunta la cámara del móvil hacia él y el cuaderno se abre aquí solo.")
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
E("s.coupled", "Practice book added", "Übeheft hinzugefügt", "Cahier ajouté", "Cuaderno añadido")

# Booklet
E("s.demo.banner", "Example with made-up data – nothing is sent.",
  "Beispiel mit ausgedachten Daten – es wird nichts gesendet.",
  "Exemple avec des données inventées – rien n'est envoyé.",
  "Ejemplo con datos inventados: no se envía nada.")
E("s.demo.end", "Close", "Beenden", "Fermer", "Cerrar")
E("s.home.refresh", "Update", "Aktualisieren", "Actualiser", "Actualizar")
E("s.home.loading", "Loading the practice book …", "Übeheft wird geladen …", "Chargement du cahier …", "Cargando el cuaderno …")
E("s.home.not_loaded", "Not loaded yet. Pull down to try again.", "Noch nicht geladen. Zum Aktualisieren nach unten ziehen.",
  "Pas encore chargé. Tire vers le bas pour réessayer.", "Aún no se ha cargado. Desliza hacia abajo para reintentar.")
E("s.home.updated", "Updated %1", "Aktualisiert %1", "Mis à jour %1", "Actualizado %1")
E("s.error.offline", "No connection – you see the last state.", "Keine Verbindung – du siehst den letzten Stand.",
  "Pas de connexion – tu vois le dernier état.", "Sin conexión: ves el último estado.")
E("s.error.gone", "%1 has ended the connection. You can remove this practice book in the settings.",
  "%1 hat die Verbindung beendet. Du kannst dieses Übeheft in den Einstellungen entfernen.",
  "%1 a mis fin à la connexion. Tu peux retirer ce cahier dans les réglages.",
  "%1 ha terminado la conexión. Puedes quitar este cuaderno en los ajustes.")
E("s.error.key", "The practice book cannot be opened. Ask your teacher for a new code.",
  "Das Übeheft lässt sich nicht öffnen. Frag deine Lehrkraft nach einem neuen Code.",
  "Impossible d'ouvrir le cahier. Demande un nouveau code à ton professeur.",
  "No se puede abrir el cuaderno. Pide un código nuevo a tu profesor.")
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
E("s.send.sent", "Sent to %1 · %2", "An %1 geschickt · %2", "Envoyé à %1 · %2", "Enviado a %1 · %2")
E("s.send.pending", "Will be sent …", "Wird gesendet …", "Envoi en cours …", "Se enviará …")
E("s.question.title", "Question to %1", "Frage an %1", "Question à %1", "Pregunta para %1")
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
E("s.settings.teachers", "Practice books", "Übehefte", "Cahiers", "Cuadernos")
E("s.settings.unknown_teacher", "Practice book", "Übeheft", "Cahier", "Cuaderno")
E("s.settings.demo", "Example", "Beispiel", "Exemple", "Ejemplo")
E("s.settings.remove", "Remove", "Entfernen", "Retirer", "Quitar")
E("s.settings.remove_confirm", "Really remove", "Wirklich entfernen", "Vraiment retirer", "Quitar de verdad")
E("s.settings.remove_hint",
  "This removes the practice book from this phone only. Your teacher can give you a new code at any time.",
  "Das entfernt das Übeheft nur von diesem Handy. Deine Lehrkraft kann dir jederzeit einen neuen Code geben.",
  "Le cahier n'est retiré que de ce téléphone. Ton professeur peut te donner un nouveau code à tout moment.",
  "Solo se quita el cuaderno de este móvil. Tu profesor puede darte un código nuevo cuando quieras.")
E("s.settings.add", "Add another teacher", "Weitere Lehrkraft hinzufügen", "Ajouter un autre professeur", "Añadir otro profesor")
E("s.settings.privacy", "Privacy", "Datenschutz", "Confidentialité", "Privacidad")
E("s.settings.privacy_text",
  "The practice book lies encrypted in your teacher's cloud. Only this app and your teacher's Qupil can read it. "
  "What you send back (practised, works, questions) goes encrypted to your teacher only. The app has no account, "
  "no advertising and no tracking.",
  "Das Übeheft liegt verschlüsselt in der Cloud deiner Lehrkraft. Lesen können es nur diese App und das Qupil deiner Lehrkraft. "
  "Was du zurückschickst (geübt, klappt, Fragen), geht verschlüsselt nur an deine Lehrkraft. Die App hat kein Konto, "
  "keine Werbung und kein Tracking.",
  "Le cahier est stocké chiffré dans le cloud de ton professeur. Seuls cette application et le Qupil de ton professeur peuvent le lire. "
  "Ce que tu renvoies (travaillé, ça marche, questions) va, chiffré, uniquement à ton professeur. L'application n'a ni compte, "
  "ni publicité, ni pistage.",
  "El cuaderno está cifrado en la nube de tu profesor. Solo esta aplicación y el Qupil de tu profesor pueden leerlo. "
  "Lo que envías (practicado, sale, preguntas) llega cifrado solo a tu profesor. La aplicación no tiene cuenta, "
  "ni publicidad, ni rastreo.")

if __name__ == "__main__":
    out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "strings.json")
    with open(out, "w", encoding="utf-8") as f:
        json.dump(S, f, ensure_ascii=False, indent=1, sort_keys=True)
        f.write("\n")
    print(len(S), "texts ->", out)
