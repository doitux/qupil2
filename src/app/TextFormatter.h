// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QColor>
#include <QObject>
#include <QPointer>
#include <QQuickTextDocument>
#include <QtQml/qqmlregistration.h>

class QTextCursor;
class QTextCharFormat;

/*!
    Bold, italic, underline and colour for a rich-text TextArea (lesson notes).

        TextArea { id: editor; textFormat: TextEdit.RichText }
        TextFormatter {
            document: editor.textDocument
            selectionStart: editor.selectionStart
            selectionEnd: editor.selectionEnd
            cursorPosition: editor.cursorPosition
        }
*/
class TextFormatter : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged FINAL)
    Q_PROPERTY(int selectionStart MEMBER m_selectionStart NOTIFY selectionChanged FINAL)
    Q_PROPERTY(int selectionEnd MEMBER m_selectionEnd NOTIFY selectionChanged FINAL)
    Q_PROPERTY(int cursorPosition MEMBER m_cursorPosition NOTIFY selectionChanged FINAL)
    Q_PROPERTY(bool bold READ bold NOTIFY formatChanged FINAL)
    Q_PROPERTY(bool italic READ italic NOTIFY formatChanged FINAL)
    Q_PROPERTY(bool underline READ underline NOTIFY formatChanged FINAL)
    Q_PROPERTY(bool colored READ colored NOTIFY formatChanged FINAL)

public:
    explicit TextFormatter(QObject *parent = nullptr);

    QQuickTextDocument *document() const { return m_document; }
    void setDocument(QQuickTextDocument *document);

    bool bold() const;
    bool italic() const;
    bool underline() const;
    bool colored() const;

    Q_INVOKABLE void toggleBold();
    Q_INVOKABLE void toggleItalic();
    Q_INVOKABLE void toggleUnderline();
    //! Marks the selection in the colour, or removes it if it already has that colour
    Q_INVOKABLE void toggleColor(const QColor &color);
    //! The note as HTML for storing (empty if there is no text)
    Q_INVOKABLE QString html() const;
    Q_INVOKABLE bool isEmpty() const;

signals:
    void documentChanged();
    void selectionChanged();
    void formatChanged();

private:
    QTextCursor cursor() const;
    QTextCharFormat currentFormat() const;
    void merge(const QTextCharFormat &format);

    QPointer<QQuickTextDocument> m_document;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
    int m_cursorPosition = 0;
};
