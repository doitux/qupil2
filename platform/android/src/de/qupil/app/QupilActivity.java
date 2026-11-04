// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.app;

import android.os.Bundle;
import android.os.SystemClock;
import android.view.View;
import android.view.ViewTreeObserver;

import org.qtproject.qt.android.bindings.QtActivity;

/*
    Qt's activity, with the launch screen held until Qupil has drawn its interface.

    Android ends the launch screen (Qupil's icon) with the activity's first frame. Qt's
    first frame is its empty container: the interface is drawn later, in a surface of its
    own. So the icon went away, the empty window showed for a moment, then Qupil. Here the
    activity does not draw until main.cpp says the first frame with content is there
    (contentReady), and the launch screen goes straight to the interface. It never waits
    longer than LONGEST_WAIT_MS.
*/
public class QupilActivity extends QtActivity
{
    private static final long LONGEST_WAIT_MS = 3000;
    private static volatile boolean s_contentReady = false;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        if (s_contentReady)
            return;   // made again while Qupil runs (Qt keeps its interface)
        final long since = SystemClock.uptimeMillis();
        final View content = findViewById(android.R.id.content);
        if (content == null)
            return;
        content.getViewTreeObserver().addOnPreDrawListener(new ViewTreeObserver.OnPreDrawListener() {
            @Override
            public boolean onPreDraw()
            {
                // Not drawn yet: Android tries again with the next frame
                if (!s_contentReady && SystemClock.uptimeMillis() - since < LONGEST_WAIT_MS)
                    return false;
                content.getViewTreeObserver().removeOnPreDrawListener(this);
                return true;
            }
        });
    }

    //! Called from main.cpp after Qupil's first frame with its interface (any thread)
    public static void contentReady()
    {
        s_contentReady = true;
    }
}
