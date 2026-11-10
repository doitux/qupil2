// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ClickGenerator.h"

#include <QRandomGenerator>
#include <QTest>

#include <cmath>

using namespace Qt::StringLiterals;

/*
    The metronome's tempo comes only from sample positions. Read the stream in blocks of
    changing size (as the audio system does) and check where each click starts.
*/
class TestMetronome : public QObject
{
    Q_OBJECT

    static QAudioFormat format(int rate)
    {
        QAudioFormat f;
        f.setSampleRate(rate);
        f.setChannelCount(1);
        f.setSampleFormat(QAudioFormat::Int16);
        return f;
    }

    //! Sample positions where a click starts (silence before, sound now)
    static QList<qint64> clickStarts(ClickGenerator &g, qint64 samples)
    {
        QList<qint64> starts;
        qint64 pos = 0;
        bool silent = true;
        QRandomGenerator random(42);
        while (pos < samples) {
            const qint64 block = std::min<qint64>(samples - pos, 64 + random.bounded(4096));
            QByteArray data(block * 2, Qt::Uninitialized);
            const qint64 got = g.read(data.data(), data.size());
            const auto *s = reinterpret_cast<const qint16 *>(data.constData());
            for (qint64 i = 0; i < got / 2; ++i, ++pos) {
                const bool sound = std::abs(s[i]) > 0;
                if (sound && silent)
                    starts << pos;
                // a click lasts 35 ms; between two clicks there is silence
                silent = !sound && (starts.isEmpty() || pos - starts.last() > 2000);
            }
        }
        return starts;
    }

private slots:
    void exactTempo_data()
    {
        QTest::addColumn<int>("rate");
        QTest::addColumn<int>("bpm");
        QTest::newRow("48 kHz, 60") << 48000 << 60;
        QTest::newRow("48 kHz, 113") << 48000 << 113;
        QTest::newRow("44.1 kHz, 97") << 44100 << 97;
        QTest::newRow("48 kHz, 240") << 48000 << 240;
    }
    void exactTempo()
    {
        QFETCH(int, rate);
        QFETCH(int, bpm);
        ClickGenerator g(format(rate));
        g.bpm.store(bpm);
        g.beatsPerBar.store(4);
        g.restartClicks();
        // Ten minutes: no click may drift by even one sample
        const qint64 samples = qint64(rate) * 600;
        const QList<qint64> starts = clickStarts(g, samples);
        const double perBeat = rate * 60.0 / bpm;
        QCOMPARE(starts.size(), qsizetype(std::ceil(samples / perBeat)));
        // (a click fades in from silence, its first sample is 0: the sound starts one later)
        for (qsizetype n = 0; n < starts.size(); ++n) {
            const qint64 expected = qint64(std::ceil(n * perBeat - 1e-9)) + 1;
            if (starts[n] != expected)
                QFAIL(qPrintable(u"click %1 at sample %2, expected %3"_s.arg(n).arg(starts[n]).arg(expected)));
        }
        // the beat shown on screen matches
        QCOMPARE(g.beatAt(qint64(100 * perBeat) + 10), qint64(100));
    }

    void tempoChangeKeepsGoing()
    {
        ClickGenerator g(format(48000));
        g.bpm.store(120);
        g.restartClicks();
        QByteArray first(48000 * 2 * 2, Qt::Uninitialized);   // 2 s = 4 beats
        g.read(first.data(), first.size());
        g.bpm.store(60);
        const QList<qint64> starts = clickStarts(g, 48000 * 4);
        // the next click comes at the old spacing, then one per second
        QVERIFY(!starts.isEmpty());
        for (qsizetype n = 1; n < starts.size(); ++n)
            QCOMPARE(starts[n] - starts[n - 1], qint64(48000));
    }
};

QTEST_GUILESS_MAIN(TestMetronome)
#include "tst_metronome.moc"
