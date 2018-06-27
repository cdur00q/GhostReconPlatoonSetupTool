#ifndef PLATOONSETUP_H
#define PLATOONSETUP_H

#include <QMainWindow>

#include <QListWidget>
#include <vector>
#include "variables.h"
#include "functions.h"
#include "actor.h"

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

    void syncSoldierPoolWithFireteam(const std::vector<actor *> &fireteam, QListWidget *fireteamList);

    void fireteamButtonClicked(std::vector<actor*> &fireteam, QListWidget *fireteamList);
};

#endif // PLATOONSETUP_H
