// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.app;

import android.content.res.Configuration;
import android.os.Build;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewTreeObserver;
import android.view.Window;
import android.view.WindowInsetsController;
import android.view.WindowManager;
import android.widget.PopupWindow;

import org.qtproject.qt.android.bindings.QtActivity;

/*
    Qt's activity, with three additions.

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

    The system's bars take Qupil's colours (setSystemBars, SystemBars_android.cpp): the
    status bar the background, the navigation bar the tab bar, with light or dark symbols.
    Qupil can be dark while the system is light (or the other way round); the theme's
    colours follow the system only, so the navigation bar stayed white under a dark Qupil.
    Qt puts the theme's colours back when it shows the window: they are set again after
    that, when the activity resumes and when it gets the focus.
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

    // The steps of the activity in Android's log (adb logcat -s Qupil), with main.cpp's
    // milestones: a window stayed black after Android had recreated the activity
    private static final String TAG = "Qupil";

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        Log.i(TAG, "activity: create, saved state " + (savedInstanceState != null) + ", content ready " + s_contentReady);
        // Always a fresh start. Android hands over a saved state when it brings back an
        // activity it had destroyed (to save memory, or "Don't keep activities"); Qt 6.8
        // has ended the whole process then (QtActivityBase.onDestroy). With that state the
        // new process said "qt started", but Qupil's main() was never called, and the
        // window stayed black (logs from a phone, 05.11.2026). Qupil keeps nothing in it.
        super.onCreate(null);
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
                if (!s_contentReady)
                    Log.w(TAG, "activity: no frame with the interface after " + LONGEST_WAIT_MS + " ms");
                content.getViewTreeObserver().removeOnPreDrawListener(this);
                return true;
            }
        });
    }

    private volatile boolean m_barsSet = false;
    private volatile int m_barTop;
    private volatile int m_barBottom;
    private volatile boolean m_barsDark;

    //! Qupil's colours for the status bar (top) and the navigation bar (bottom), any thread
    public void setSystemBars(int top, int bottom, boolean dark)
    {
        m_barTop = top;
        m_barBottom = bottom;
        m_barsDark = dark;
        m_barsSet = true;
        runOnUiThread(() -> {
            applySystemBars();
            // Once more after Qt has shown the window (it restores the theme's colours then)
            getWindow().getDecorView().postDelayed(this::applySystemBars, 600);
        });
    }

    private void applySystemBars()
    {
        if (!m_barsSet || isFinishing())
            return;
        final Window window = getWindow();
        window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
        window.setStatusBarColor(m_barTop);
        window.setNavigationBarColor(m_barBottom);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            window.setStatusBarContrastEnforced(false);
            window.setNavigationBarContrastEnforced(false);
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            final WindowInsetsController insets = window.getInsetsController();
            if (insets != null) {
                final int light = WindowInsetsController.APPEARANCE_LIGHT_STATUS_BARS
                                  | WindowInsetsController.APPEARANCE_LIGHT_NAVIGATION_BARS;
                insets.setSystemBarsAppearance(m_barsDark ? 0 : light, light);
            }
        } else {
            final View decor = window.getDecorView();
            final int light = View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR | View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR;
            final int flags = decor.getSystemUiVisibility();
            decor.setSystemUiVisibility(m_barsDark ? flags & ~light : flags | light);
        }
    }

    //! Nothing to restore (see onCreate); Qt itself only restores after a configuration
    //! change, which Qupil handles without a new activity (configChanges in the manifest)
    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState)
    {
        Log.i(TAG, "activity: saved state not restored");
    }

    @Override
    protected void onStart()
    {
        Log.i(TAG, "activity: start");
        super.onStart();
    }

    @Override
    protected void onResume()
    {
        Log.i(TAG, "activity: resume, content ready " + s_contentReady);
        super.onResume();
        applySystemBars();
    }

    @Override
    protected void onStop()
    {
        Log.i(TAG, "activity: stop");
        super.onStop();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        Log.i(TAG, "activity: focus " + hasFocus);
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus)
            applySystemBars();
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
        Log.i(TAG, "activity: destroy, finishing " + isFinishing() + ", changing configurations " + isChangingConfigurations());
        if (m_keyboardProbe != null) {
            m_keyboardProbe.dismiss();
            m_keyboardProbe = null;
        }
        super.onDestroy();
    }

    //! Called from main.cpp after Qupil's first frame with its interface (any thread)
    public static void contentReady()
    {
        Log.i(TAG, "activity: content ready");
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
