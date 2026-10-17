// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>

class QQuickWindow;

/*!
    The system back button (Android: the arrow at the bottom, or the gesture)
    goes back one step in Qupil instead of closing it: the window's QML function
    goBack() decides (closes the page on top, then back to the first tab). An
    open menu, dialog or editor is closed by the popup itself before. On the
    first screen Qupil goes to the background like with the home button, so it
    is still where it was when opened again.
*/
class BackKey : public QObject
{
public:
    explicit BackKey(QQuickWindow *window);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QQuickWindow *m_window;
    bool m_pressed = false;
};
