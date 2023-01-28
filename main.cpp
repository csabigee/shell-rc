#include "mainwindow.h"
#include "DarkBlueSheet.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new DarkBlueSheet);

    MainWindow w;
    w.show();
    return a.exec();
}
