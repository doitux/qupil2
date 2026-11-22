// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: StudentActivity.startScan(prompt) opens the scanner of zxing-android-embedded;
// its result comes back through StudentActivity.scanResult (Android's main thread).

#include "QrScanner.h"

#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>

#include <mutex>

namespace QrScanner {

namespace {

std::mutex s_mutex;
std::function<void(const Result &)> s_done;
bool s_registered = false;

void JNICALL scanResult(JNIEnv *env, jclass, jstring text, jboolean denied)
{
    std::function<void(const Result &)> done;
    {
        const std::lock_guard<std::mutex> lock(s_mutex);
        done.swap(s_done);
    }
    if (!done)
        return;
    Result result;
    if (text) {
        const char *utf8 = env->GetStringUTFChars(text, nullptr);
        if (utf8) {
            result.text = QString::fromUtf8(utf8);
            env->ReleaseStringUTFChars(text, utf8);
        }
    }
    result.denied = denied;
    done(result);
}

bool registerResult()
{
    if (s_registered)
        return true;
    const JNINativeMethod methods[] = {
        { "scanResult", "(Ljava/lang/String;Z)V", reinterpret_cast<void *>(scanResult) },
    };
    QJniEnvironment env;
    s_registered = env.registerNativeMethods("de/qupil/student/StudentActivity", methods, 1);
    if (!s_registered)
        qWarning("QR scanner: StudentActivity not found");
    return s_registered;
}

} // namespace

bool nativeAvailable()
{
    return true;
}

void nativeScan(const Texts &texts, std::function<void(const Result &)> done)
{
    QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!registerResult() || !activity.isValid()) {
        done({});
        return;
    }
    {
        const std::lock_guard<std::mutex> lock(s_mutex);
        s_done = std::move(done);
    }
    activity.callMethod<void>("startScan", "(Ljava/lang/String;)V",
                              QJniObject::fromString(texts.prompt).object<jstring>());
}

} // namespace QrScanner
