#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Gamepad w;
    w.show();
    return a.exec();
}
