#include "NesEmulateWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NesEmulateWindow w;
    w.show();

    return a.exec();
}
