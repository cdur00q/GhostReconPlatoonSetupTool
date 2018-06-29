#ifndef PLATOONSETUP_H
#define PLATOONSETUP_H

#include <QMainWindow>

#include <QListWidget>
#include <QPushButton>
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

    void on_pbBravo_clicked();

    void on_pbCharlie_clicked();

    void on_lwSoldierPool_itemClicked();

    void on_lwAlpha_itemClicked();

    void on_lwBravo_itemClicked();

    void on_lwCharlie_itemClicked();

    void on_pbUnassign_clicked();

private:
    Ui::PlatoonSetup *ui;

    void syncSoldierPoolWithFireteam(const std::vector<actor *> &fireteam, QListWidget *fireteamList);
    void syncFireteamWithSoldierPool(const std::vector<actor *> &fireteam, const QListWidget *fireteamList);

    void fireteamButtonClicked(std::vector<actor*> &fireteam, QListWidget *fireteamList);

    void unassignButtonClicked(std::vector<actor*> &fireteam, QListWidget *fireteamList);

    void updateTeamButton(std::vector<actor*> &fireteam, QPushButton *teamButton);

    void updateUnassignButton();
};

#endif // PLATOONSETUP_H
