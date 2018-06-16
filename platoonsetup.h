#ifndef PLATOONSETUP_H
#define PLATOONSETUP_H

#include <QMainWindow>

namespace Ui {
class PlatoonSetup;
}

class PlatoonSetup : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlatoonSetup(QWidget *parent = 0);
    ~PlatoonSetup();

private:
    Ui::PlatoonSetup *ui;
};

#endif // PLATOONSETUP_H
