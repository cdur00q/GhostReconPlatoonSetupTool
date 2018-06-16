#include "platoonsetup.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlatoonSetup w;
    w.show();

    return a.exec();
}
