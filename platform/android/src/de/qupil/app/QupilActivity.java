// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.app;

import android.content.res.Configuration;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.SystemClock;
import android.view.Gravity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.WindowManager;
import android.widget.PopupWindow;

import org.qtproject.qt.android.bindings.QtActivity;

/*
    Qt's activity, with two additions.

    The launch screen is held until Qupil has drawn its interface. Android ends the launch
    screen (Qupil's icon) with the activity's first frame. Qt's first frame is its empty
    container: the interface is drawn later, in a surface of its own. So the icon went away,
    the empty window showed for a moment, then Qupil. Here the activity does not draw until
    main.cpp says the first frame with content is there (contentReady), and the launch screen
    goes straight to the interface. It never waits longer than LONGEST_WAIT_MS.

    The on-screen keyboard is measured. The window keeps its size when the keyboard comes
    (adjustNothing in the manifest): when Android made it smaller and larger again, Qt's
    surface was empty or showed the old picture for a frame each time, a white or black
    flicker. An invisible, zero-width popup window does resize with the keyboard; its
    visible height gives the keyboard's height, which goes to Qupil
    (keyboardHeightChanged, KeyboardHeight_android.cpp), and the interface makes room.
*/
public class QupilActivity extends QtActivity
{
    private static final long LONGEST_WAIT_MS = 3000;
    private static volatile boolean s_contentReady = false;

    private static native void keyboardHeightChanged(int px);
    private static volatile int s_keyboardPx = 0;
    private PopupWindow m_keyboardProbe;
    //! Bottom of the visible area without a keyboard
    private int m_probeBottom = 0;

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        startKeyboardProbe();
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

    @Override
    public void onConfigurationChanged(Configuration newConfig)
    {
        super.onConfigurationChanged(newConfig);
        m_probeBottom = 0;   // turned: measured anew
    }

    @Override
    protected void onDestroy()
    {
        if (m_keyboardProbe != null) {
            m_keyboardProbe.dismiss();
            m_keyboardProbe = null;
        }
        super.onDestroy();
    }

    //! Called from main.cpp after Qupil's first frame with its interface (any thread)
    public static void contentReady()
    {
        s_contentReady = true;
        if (s_keyboardPx > 0)
            report(s_keyboardPx);
    }

    private static void report(int px)
    {
        try {
            keyboardHeightChanged(px);
        } catch (UnsatisfiedLinkError e) {
            // Qupil's native part not there yet: it asks again (contentReady)
        }
    }

    private void startKeyboardProbe()
    {
        try {
            final View decor = getWindow().getDecorView();
            final View probe = new View(this);
            m_keyboardProbe = new PopupWindow(probe, 0, WindowManager.LayoutParams.MATCH_PARENT);
            m_keyboardProbe.setBackgroundDrawable(new ColorDrawable(0));
            m_keyboardProbe.setSoftInputMode(WindowManager.LayoutParams.SOFT_INPUT_ADJUST_RESIZE
                                             | WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);
            m_keyboardProbe.setInputMethodMode(PopupWindow.INPUT_METHOD_NEEDED);
            m_keyboardProbe.setFocusable(false);
            m_keyboardProbe.setTouchable(false);
            probe.getViewTreeObserver().addOnGlobalLayoutListener(() -> {
                final Rect visible = new Rect();
                probe.getWindowVisibleDisplayFrame(visible);
                if (visible.bottom > m_probeBottom)
                    m_probeBottom = visible.bottom;
                final int px = Math.max(0, m_probeBottom - visible.bottom);
                if (px == s_keyboardPx)
                    return;
                s_keyboardPx = px;
                if (s_contentReady)
                    report(px);
            });
            decor.post(() -> {
                if (m_keyboardProbe != null && !isFinishing() && decor.getWindowToken() != null)
                    m_keyboardProbe.showAtLocation(decor, Gravity.NO_GRAVITY, 0, 0);
            });
        } catch (RuntimeException e) {
            m_keyboardProbe = null;   // without it the keyboard covers the bottom (as a fallback)
        }
    }
}
