// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "QrCode.h"

#include "qrcodegen.hpp"

#include <QPainter>

using namespace Qt::StringLiterals;

namespace Qupil::QrCode {

namespace {
constexpr int kQuietZone = 4;
}

Code encode(const QString &text)
{
    Code code;
    try {
        const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(text.toUtf8().constData(), qrcodegen::QrCode::Ecc::MEDIUM);
        code.size = qr.getSize();
        code.modules.reserve(code.size * code.size);
        for (int y = 0; y < code.size; ++y)
            for (int x = 0; x < code.size; ++x)
                code.modules.append(qr.getModule(x, y));
    } catch (const std::exception &) {
        return {};
    }
    return code;
}

QImage toImage(const Code &code, int scale)
{
    if (code.isEmpty() || scale < 1)
        return {};
    const int side = (code.size + 2 * kQuietZone) * scale;
    QImage image(side, side, QImage::Format_RGB32);
    image.fill(Qt::white);
    QPainter p(&image);
    for (int y = 0; y < code.size; ++y)
        for (int x = 0; x < code.size; ++x)
            if (code.dark(x, y))
                p.fillRect((x + kQuietZone) * scale, (y + kQuietZone) * scale, scale, scale, Qt::black);
    return image;
}

QString toSvgPath(const Code &code)
{
    // Runs of dark modules per row as rectangles: short, and no seams between modules
    QString path;
    for (int y = 0; y < code.size; ++y) {
        for (int x = 0; x < code.size;) {
            if (!code.dark(x, y)) {
                ++x;
                continue;
            }
            int run = 1;
            while (x + run < code.size && code.dark(x + run, y))
                ++run;
            path += u"M%1 %2h%3v1h-%3z"_s.arg(x + kQuietZone).arg(y + kQuietZone).arg(run);
            x += run;
        }
    }
    return path;
}

} // namespace Qupil::QrCode
