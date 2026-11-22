// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
package de.qupil.student;

import android.content.Intent;
import android.util.Log;

import com.google.zxing.integration.android.IntentIntegrator;

import org.qtproject.qt.android.bindings.QtActivity;

/*!
    Qt's activity plus the QR code scanner for the coupling code.

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
