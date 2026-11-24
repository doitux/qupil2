// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop and Android: the window ends at the system's bars, nothing to keep clear of.

#include "SafeArea.h"

namespace SafeArea {

void watch(QWindow *, std::function<void(qreal, qreal)>)
{
}

} // namespace SafeArea
