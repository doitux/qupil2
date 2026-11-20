// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QImage>
#include <QString>

namespace Qupil::QrCode {

/*!
    The QR code of \a text (the coupling link), error correction "medium" (a crease or a
    reflection on the screen still reads). Empty on failure (text too long).
*/
struct Code
{
    int size = 0;                  // modules per side, without the quiet zone
    QList<bool> modules;           // row by row, true = dark
    bool isEmpty() const { return size == 0; }
    bool dark(int x, int y) const { return modules.at(y * size + x); }
};
Code encode(const QString &text);

//! As a picture: \a scale pixels per module, a quiet zone of 4 modules, black on white
QImage toImage(const Code &code, int scale = 8);
//! As an SVG path (one unit per module, quiet zone included in the view box) for Shape in QML
QString toSvgPath(const Code &code);

} // namespace Qupil::QrCode
