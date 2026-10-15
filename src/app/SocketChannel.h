// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QTcpSocket>
#include <QtEndian>

#include "WifiSession.h"

/*!
    Frames over TCP for the Wi-Fi sync: 4 bytes length (big-endian), then the
    frame. Blocking; create and use it in a worker thread.
*/
class SocketChannel : public Qupil::FrameChannel
{
public:
    static constexpr int TimeoutMs = 20000;
    static constexpr quint32 MaxFrame = 64 * 1024 * 1024;

    explicit SocketChannel(QTcpSocket *socket)
        : m_socket(socket)
    {
    }

    bool send(const QByteArray &frame) override
    {
        char length[4];
        qToBigEndian(quint32(frame.size()), length);
        if (m_socket->write(length, 4) != 4 || m_socket->write(frame) != frame.size())
            return false;
        while (m_socket->bytesToWrite() > 0) {
            if (!m_socket->waitForBytesWritten(TimeoutMs))
                return false;
        }
        return true;
    }

    std::optional<QByteArray> receive() override
    {
        const auto header = readExactly(4);
        if (!header)
            return std::nullopt;
        const quint32 length = qFromBigEndian<quint32>(header->constData());
        if (length > MaxFrame)
            return std::nullopt;
        return readExactly(int(length));
    }

private:
    std::optional<QByteArray> readExactly(int size)
    {
        QByteArray data;
        data.reserve(size);
        while (data.size() < size) {
            if (m_socket->bytesAvailable() == 0 && !m_socket->waitForReadyRead(TimeoutMs))
                return std::nullopt;
            data += m_socket->read(size - data.size());
        }
        return data;
    }

    QTcpSocket *m_socket;
};
