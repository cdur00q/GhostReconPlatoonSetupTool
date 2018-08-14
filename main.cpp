#include "platoonsetup.h"
#include <QApplication>

#include "functions.h"
#include <cstdlib> // for std::srand()
#include <ctime> // for std::time()

int main(int argc, char *argv[])
{
    std::srand(static_cast<unsigned int>(std::time(0))); // set initial seed value to system clock
    getRandomNumber(0, 99);  // run the random number function once to for fix issue
                             // with first soldier not being randomized

    QApplication a(argc, argv);
    PlatoonSetup w;
    //connect(a, SIGNAL(a.applicationStateChanged(Qt::ApplicationInactive)), w, SLOT(w.close()));
    w.show();

    return a.exec();
}
