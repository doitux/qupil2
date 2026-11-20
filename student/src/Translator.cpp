// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "Translator.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

using namespace Qt::StringLiterals;

bool Translator::load(const QString &resource, const QString &language)
{
    QFile file(resource);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    const QJsonObject all = QJsonDocument::fromJson(file.readAll()).object();
    m_texts.clear();
    for (auto it = all.constBegin(); it != all.constEnd(); ++it) {
        const QJsonObject languages = it.value().toObject();
        QJsonValue v = languages.value(language);
        if (v.isUndefined() || v.toString().isEmpty() && !v.isObject())
            v = languages.value(u"en"_s);
        Text text;
        if (v.isObject()) {
            text.one = v.toObject().value(u"one"_s).toString();
            text.other = v.toObject().value(u"other"_s).toString();
        } else {
            text.one = text.other = v.toString();
        }
        m_texts.insert(it.key().toUtf8(), text);
    }
    return !m_texts.isEmpty();
}

QString Translator::translate(const char *, const char *sourceText, const char *, int n) const
{
    // qtTrId() asks with the ID as source text; QCoreApplication replaces %n afterwards
    const auto it = m_texts.constFind(QByteArray(sourceText));
    if (it == m_texts.constEnd())
        return {};
    return n == 1 ? it->one : it->other;
}
