// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Mdns.h"

#include <QStringList>
#include <QtEndian>

namespace Qupil::Mdns {

namespace {

void put16(QByteArray &out, quint16 v)
{
    char b[2];
    qToBigEndian(v, b);
    out.append(b, 2);
}

void put32(QByteArray &out, quint32 v)
{
    char b[4];
    qToBigEndian(v, b);
    out.append(b, 4);
}

// Without compression: simpler, and every receiver understands it
void putName(QByteArray &out, const QString &name)
{
    // Labels are separated by dots; an instance name never contains one here
    for (const QString &label : name.split(u'.', Qt::SkipEmptyParts)) {
        const QByteArray l = label.toUtf8().left(63);
        out.append(char(l.size()));
        out.append(l);
    }
    out.append('\0');
}

void putRecord(QByteArray &out, const Record &r)
{
    putName(out, r.name);
    put16(out, r.type);
    put16(out, quint16(1 | (r.cacheFlush ? 0x8000 : 0)));
    put32(out, r.ttl);
    QByteArray data;
    switch (r.type) {
    case PTR:
        putName(data, r.target);
        break;
    case SRV:
        put16(data, 0);        // priority
        put16(data, 0);        // weight
        put16(data, r.port);
        putName(data, r.target);
        break;
    case TXT:
        for (const QByteArray &t : r.txt) {
            const QByteArray s = t.left(255);
            data.append(char(s.size()));
            data.append(s);
        }
        if (r.txt.isEmpty())
            data.append('\0');
        break;
    case A:
    case AAAA:
        data = r.address;
        break;
    default:
        break;
    }
    put16(out, quint16(data.size()));
    out.append(data);
}

class Reader
{
public:
    explicit Reader(const QByteArray &data)
        : m_data(data)
    {
    }

    bool ok() const { return m_ok; }
    int position() const { return m_pos; }
    void seek(int pos) { m_pos = pos; }

    quint16 u16()
    {
        if (m_pos + 2 > m_data.size())
            return fail(), 0;
        const quint16 v = qFromBigEndian<quint16>(m_data.constData() + m_pos);
        m_pos += 2;
        return v;
    }
    quint32 u32()
    {
        if (m_pos + 4 > m_data.size())
            return fail(), 0;
        const quint32 v = qFromBigEndian<quint32>(m_data.constData() + m_pos);
        m_pos += 4;
        return v;
    }
    QByteArray bytes(int n)
    {
        if (n < 0 || m_pos + n > m_data.size())
            return fail(), QByteArray();
        const QByteArray v = m_data.mid(m_pos, n);
        m_pos += n;
        return v;
    }

    //! A name, following compression pointers (with a limit against loops)
    QString name()
    {
        QStringList labels;
        int pos = m_pos;
        int jumps = 0;
        bool jumped = false;
        while (true) {
            if (pos >= m_data.size())
                return fail(), QString();
            const quint8 len = quint8(m_data.at(pos));
            if (len == 0) {
                ++pos;
                break;
            }
            if ((len & 0xC0) == 0xC0) {
                if (pos + 1 >= m_data.size() || ++jumps > 32)
                    return fail(), QString();
                const int target = ((len & 0x3F) << 8) | quint8(m_data.at(pos + 1));
                if (!jumped)
                    m_pos = pos + 2;
                jumped = true;
                pos = target;
                continue;
            }
            if (len > 63 || pos + 1 + len > m_data.size())
                return fail(), QString();
            labels << QString::fromUtf8(m_data.mid(pos + 1, len));
            pos += 1 + len;
        }
        if (!jumped)
            m_pos = pos;
        return labels.join(u'.');
    }

private:
    void fail() { m_ok = false; }

    const QByteArray &m_data;
    int m_pos = 0;
    bool m_ok = true;
};

std::optional<Record> readRecord(Reader &in)
{
    Record r;
    r.name = in.name();
    r.type = in.u16();
    const quint16 cls = in.u16();
    r.cacheFlush = cls & 0x8000;
    r.ttl = in.u32();
    const int length = in.u16();
    if (!in.ok())
        return std::nullopt;
    const int start = in.position();
    switch (r.type) {
    case PTR:
        r.target = in.name();
        break;
    case SRV:
        in.u16();   // priority
        in.u16();   // weight
        r.port = in.u16();
        r.target = in.name();
        break;
    case TXT: {
        int left = length;
        while (left > 0 && in.ok()) {
            const QByteArray len = in.bytes(1);
            if (len.isEmpty())
                break;
            const int n = quint8(len.at(0));
            if (n > 0)
                r.txt << in.bytes(n);
            left -= 1 + n;
        }
        break;
    }
    case A:
    case AAAA:
        r.address = in.bytes(length);
        break;
    default:
        break;
    }
    in.seek(start + length);
    if (!in.ok())
        return std::nullopt;
    return r;
}

} // namespace

QByteArray encode(const Message &m)
{
    QByteArray out;
    put16(out, m.id);
    put16(out, m.response ? 0x8400 : 0);   // response + authoritative
    put16(out, quint16(m.questions.size()));
    put16(out, quint16(m.answers.size()));
    put16(out, 0);
    put16(out, quint16(m.additional.size()));
    for (const Question &q : m.questions) {
        putName(out, q.name);
        put16(out, q.type);
        put16(out, quint16(1 | (q.unicastResponse ? 0x8000 : 0)));
    }
    for (const Record &r : m.answers)
        putRecord(out, r);
    for (const Record &r : m.additional)
        putRecord(out, r);
    return out;
}

std::optional<Message> decode(const QByteArray &packet)
{
    if (packet.size() < 12)
        return std::nullopt;
    Reader in(packet);
    Message m;
    m.id = in.u16();
    const quint16 flags = in.u16();
    m.response = flags & 0x8000;
    const int questions = in.u16();
    const int answers = in.u16();
    const int authority = in.u16();
    const int additional = in.u16();
    if (questions + answers + authority + additional > 400)
        return std::nullopt;
    for (int i = 0; i < questions && in.ok(); ++i) {
        Question q;
        q.name = in.name();
        q.type = in.u16();
        q.unicastResponse = in.u16() & 0x8000;
        m.questions << q;
    }
    for (int i = 0; i < answers + authority + additional; ++i) {
        const auto r = readRecord(in);
        if (!r)
            return std::nullopt;
        (i < answers ? m.answers : m.additional) << *r;
    }
    if (!in.ok())
        return std::nullopt;
    return m;
}

bool sameName(const QString &a, const QString &b)
{
    QString x = a, y = b;
    if (x.endsWith(u'.'))
        x.chop(1);
    if (y.endsWith(u'.'))
        y.chop(1);
    return x.compare(y, Qt::CaseInsensitive) == 0;
}

QString txtValue(const QList<QByteArray> &txt, const QByteArray &key)
{
    for (const QByteArray &t : txt) {
        const qsizetype eq = t.indexOf('=');
        if (eq > 0 && t.left(eq).compare(key, Qt::CaseInsensitive) == 0)
            return QString::fromUtf8(t.mid(eq + 1));
    }
    return {};
}

} // namespace Qupil::Mdns
