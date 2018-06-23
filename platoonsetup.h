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

private slots:
    void on_pbAlpha_clicked();

    void on_lwSoldierPool_itemSelectionChanged();

    void on_lwAlpha_itemSelectionChanged();

    void on_pbUnassign_clicked();

private:
    Ui::PlatoonSetup *ui;
};

#endif // PLATOONSETUP_H
