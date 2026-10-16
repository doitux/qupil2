// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "TextPack.h"

#include <QByteArray>

using namespace Qt::StringLiterals;

namespace TextPack {

namespace {
const QString kPrefix = u"qz1:"_s;
}

bool isPacked(const QString &text)
{
    return text.startsWith(kPrefix);
}

QString pack(const QString &text)
{
    if (text.isEmpty() || isPacked(text))
        return text;
    return kPrefix + QString::fromLatin1(qCompress(text.toUtf8(), 9).toBase64());
}

QString unpack(const QString &text)
{
    if (!isPacked(text))
        return text;
    return QString::fromUtf8(qUncompress(QByteArray::fromBase64(text.mid(kPrefix.size()).toLatin1())));
}

} // namespace TextPack
