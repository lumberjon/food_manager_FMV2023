#ifndef CUSTOM_LINE_EDIT_H
#define CUSTOM_LINE_EDIT_H

#include <QApplication>
#include <QLineEdit>
#include <QKeyEvent>



class CustomLineEdit : public QLineEdit
{
public:
    explicit CustomLineEdit(QWidget *parent = nullptr)
        : QLineEdit(parent)
    {
    }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        {
            // Ignore the Enter key event
            event->ignore();
            return;
        }

        // Call the base class implementation for other key events
        QLineEdit::keyPressEvent(event);
    }

    void inputMethodEvent(QInputMethodEvent *event) override
    {
        // Ignore carriage return and line feed characters from input method
        QString text = event->commitString();
        text.remove(QChar('\r'));
        text.remove(QChar('\n'));
        event->setCommitString(text);
        QLineEdit::inputMethodEvent(event);
    }
};



#endif // CUSTOM_LINE_EDIT_H
