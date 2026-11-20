// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop: no share menu; AppController copies the text instead.

#include "TextShare.h"

namespace TextShare {

bool available()
{
    return false;
}

bool share(const QString &text, const QString &title)
{
    (void)text;
    (void)title;
    return false;
}

} // namespace TextShare
