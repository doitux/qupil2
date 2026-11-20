// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: the share menu (Intent.ACTION_SEND with text/plain, in a chooser).

#include "TextShare.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QtCore/qcoreapplication_platform.h>

namespace TextShare {

bool available()
{
    return true;
}

bool share(const QString &text, const QString &title)
{
    const QJniObject context = QNativeInterface::QAndroidApplication::context();
    if (!context.isValid())
        return false;
    QJniObject intent("android/content/Intent");
    if (!intent.isValid())
        return false;
    const auto str = [](const QString &s) { return QJniObject::fromString(s); };
    intent.callObjectMethod("setAction", "(Ljava/lang/String;)Landroid/content/Intent;",
                            str(QStringLiteral("android.intent.action.SEND")).object<jstring>());
    intent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;",
                            str(QStringLiteral("text/plain")).object<jstring>());
    intent.callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                            str(QStringLiteral("android.intent.extra.TEXT")).object<jstring>(),
                            str(text).object<jstring>());
    if (!title.isEmpty())
        intent.callObjectMethod("putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;",
                                str(QStringLiteral("android.intent.extra.SUBJECT")).object<jstring>(),
                                str(title).object<jstring>());
    QJniObject chooser = QJniObject::callStaticObjectMethod(
        "android/content/Intent", "createChooser",
        "(Landroid/content/Intent;Ljava/lang/CharSequence;)Landroid/content/Intent;",
        intent.object(), str(title).object<jstring>());
    if (!chooser.isValid())
        return false;
    // The context may be the application rather than the activity
    const jint newTask = 0x10000000;   // Intent.FLAG_ACTIVITY_NEW_TASK
    chooser.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", newTask);
    context.callMethod<void>("startActivity", "(Landroid/content/Intent;)V", chooser.object());
    QJniEnvironment env;
    return !env.checkAndClearExceptions();
}

} // namespace TextShare
