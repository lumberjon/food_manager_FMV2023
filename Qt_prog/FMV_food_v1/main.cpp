#include "mainwindow.h"

#include <QApplication>

#include <focus_filter.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setWindowFlags( Qt::WindowStaysOnTopHint );
    w.show();

    return a.exec();
}


