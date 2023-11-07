#ifndef FOCUS_FILTER_H
#define FOCUS_FILTER_H

#include <QApplication>
#include <QLineEdit>
#include <QMainWindow>
#include <QEvent>



class FocusFilter : public QObject
{
    Q_OBJECT

public:
    explicit FocusFilter(QLineEdit *lineEdit, QObject *parent = nullptr)
        : QObject(parent), lineEdit(lineEdit)
    {
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::FocusOut)
        {
            lineEdit->setFocus();
            return true;
        }
        return QObject::eventFilter(obj, event);
    }

private:
    QLineEdit *lineEdit;
};



#endif // FOCUS_FILTER_H
