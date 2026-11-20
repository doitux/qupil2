// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Texts by ID (qsTrId) from i18n/strings.json, in the language of the phone.
// English when a language is missing.

#pragma once

#include <QHash>
#include <QTranslator>

class Translator : public QTranslator
{
public:
    bool load(const QString &resource, const QString &language);
    QString translate(const char *context, const char *sourceText, const char *disambiguation = nullptr,
                      int n = -1) const override;
    bool isEmpty() const override { return m_texts.isEmpty(); }

private:
    struct Text { QString one, other; };
    QHash<QByteArray, Text> m_texts;
};
