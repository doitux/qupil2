// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

/*!
    Hands a text to the system's share menu (Messages, WhatsApp, Signal, Mail …): the
    homework of a lesson for a student without the practice book app. iOS and Android
    have one; on desktop systems available() is false and Qupil copies the text instead.
*/
namespace TextShare {

bool available();

//! Opens the share menu; \a title is the subject where an app uses one (mail).
//! false: there is no share menu (or it could not open)
bool share(const QString &text, const QString &title);

} // namespace TextShare
