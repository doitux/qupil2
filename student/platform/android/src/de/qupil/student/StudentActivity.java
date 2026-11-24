// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.student;

import android.content.Intent;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.WindowManager;
import android.widget.PopupWindow;

import com.google.zxing.integration.android.IntentIntegrator;

import org.qtproject.qt.android.bindings.QtActivity;

/*!
    Qt's activity plus the links that open the app and the QR code scanner.

    A coupling link (https://qupil.de/u#… from the phone's camera or a message, or
    qupil-student:…) comes as an intent: the one the app was started with waits until
    Qupil's native part asks for it (takePendingLink), later ones go straight to
    linkReceived(). Qt's own way (QDesktopServices) did not hand them on: the link opened
    the app and nothing happened.

    startScan() opens the scanner of zxing-android-embedded (its own activity; it asks for
    the camera permission itself and reads the code on the phone). The result goes to
    Qupil's native part through scanResult(): the text of the code, "" when the scanner
    was closed without one, and whether the camera was refused.

    The on-screen keyboard is measured, as in Qupil (QupilActivity). The window keeps its
    size when the keyboard comes (adjustNothing in the manifest): when Android made it
    smaller and larger again, the page jumped and was empty for a moment each time. An
    invisible, zero-width popup window does resize with the keyboard; its visible height
    gives the keyboard's height, which goes to the app (keyboardHeightChanged,
    KeyboardHeight_android.cpp), and the page makes room itself.
*/
public class StudentActivity extends QtActivity
{
    private static final String TAG = "QupilStudent";
    private static final int SCAN_REQUEST = 0x5143;   // "QC"

    private static native void scanResult(String text, boolean denied);
    private static native void linkReceived(String link);

    private static String s_pendingLink = null;
    private static boolean s_nativeReady = false;

    private static native void keyboardHeightChanged(int px);
    private static volatile int s_keyboardPx = 0;
    private static volatile boolean s_keyboardWatched = false;
    private PopupWindow m_keyboardProbe;
    //! Bottom of the visible area without a keyboard
    private int m_probeBottom = 0;

    private static synchronized void offer(Intent intent)
    {
        if (intent == null || !Intent.ACTION_VIEW.equals(intent.getAction()) || intent.getDataString() == null)
            return;
        // Brought back from the list of recent apps: the link was handled then
        if ((intent.getFlags() & Intent.FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY) != 0)
            return;
        final String link = intent.getDataString();
        // Never the link itself in the log: it is the key
        if (s_nativeReady) {
            Log.i(TAG, "a link came in");
            linkReceived(link);
        } else {
            Log.i(TAG, "a link came in (kept until the app is ready)");
            s_pendingLink = link;
        }
    }

    //! Called by Qupil's native part once it takes links: the one the app was started with (or null)
    public static synchronized String takePendingLink()
    {
        s_nativeReady = true;
        final String link = s_pendingLink;
        s_pendingLink = null;
        return link;
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        // Not when Android rebuilds the activity: that intent was handled already
        if (savedInstanceState == null)
            offer(getIntent());
        startKeyboardProbe();
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

    //! Called by the app's native part once it listens (KeyboardHeight_android.cpp, any thread)
    public static void keyboardWatched()
    {
        s_keyboardWatched = true;
        if (s_keyboardPx > 0)
            report(s_keyboardPx);
    }

    private static void report(int px)
    {
        try {
            keyboardHeightChanged(px);
        } catch (UnsatisfiedLinkError e) {
            // The native part is not there yet: it asks again (keyboardWatched)
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
                Log.i(TAG, "keyboard: " + px + " px");
                if (s_keyboardWatched)
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

    @Override
    public void onNewIntent(Intent intent)
    {
        super.onNewIntent(intent);
        offer(intent);
    }

    //! Called from Qt's thread (QrScanner_android.cpp)
    public void startScan(final String prompt)
    {
        runOnUiThread(new Runnable() {
            @Override
            public void run()
            {
                try {
                    IntentIntegrator integrator = new IntentIntegrator(StudentActivity.this);
                    integrator.setDesiredBarcodeFormats(IntentIntegrator.QR_CODE);
                    integrator.setPrompt(prompt);
                    integrator.setBeepEnabled(false);
                    integrator.setOrientationLocked(false);
                    startActivityForResult(integrator.createScanIntent(), SCAN_REQUEST);
                    Log.i(TAG, "scanner opened");
                } catch (RuntimeException e) {
                    Log.w(TAG, "scanner could not be opened", e);
                    scanResult("", false);
                }
            }
        });
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        if (requestCode != SCAN_REQUEST) {
            super.onActivityResult(requestCode, resultCode, data);
            return;
        }
        String text = "";
        boolean denied = false;
        if (data != null) {
            if (resultCode == RESULT_OK && data.getStringExtra("SCAN_RESULT") != null)
                text = data.getStringExtra("SCAN_RESULT");
            denied = data.getBooleanExtra("MISSING_CAMERA_PERMISSION", false);
        }
        Log.i(TAG, "scanner closed: " + (text.isEmpty() ? (denied ? "camera refused" : "no code") : "a code"));
        scanResult(text, denied);
    }
}
