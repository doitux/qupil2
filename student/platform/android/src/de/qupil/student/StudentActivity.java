// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.student;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

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
*/
public class StudentActivity extends QtActivity
{
    private static final String TAG = "QupilStudent";
    private static final int SCAN_REQUEST = 0x5143;   // "QC"

    private static native void scanResult(String text, boolean denied);
    private static native void linkReceived(String link);

    private static String s_pendingLink = null;
    private static boolean s_nativeReady = false;

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
