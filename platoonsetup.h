#ifndef PLATOONSETUP_H
#define PLATOONSETUP_H

#include <QMainWindow>

#include <QListWidget>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <vector>
#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "kit.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "strings.h"
#include "assignedkitmap.h"

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

    void on_lwKits_itemClicked();

    void on_pbKitLeft_clicked();

    void on_pbKitRight_clicked();

    void on_pbKitFireteam_clicked();

    void on_pbKitSquad_clicked();

    void on_pbApply_clicked();

    void mediaPlayerStateChanged(QMediaPlayer::State state);

    void qApplicationStateChanged(Qt::ApplicationState state);

private:
    Ui::PlatoonSetup *ui;

    std::vector<Actor> m_rifleman;
    std::vector<Actor> m_heavyWeapons;
    std::vector<Actor> m_sniper;
    std::vector<Actor> m_demolitions;
    std::vector<Actor> m_actors;
    std::vector<Actor*> m_alpha; // these point to elements in actors, which lasts until the end of the program so no need to worry about dangling pointers // should these pointer vectors be const in some way?
    std::vector<Actor*> m_bravo;
    std::vector<Actor*> m_charlie;
    std::vector<Kit> m_riflemanKits;
    std::vector<Kit> m_heavyWeaponsKits;
    std::vector<Kit> m_sniperKits;
    std::vector<Kit> m_demolitionsKits;
    std::vector<Gun> m_guns;
    std::vector<Projectile> m_projectiles;
    std::vector<Item> m_items;
    Strings m_strings;
    AssignedKitMap m_assignedKitMap;
    QMediaPlayer *m_mediaPlayer = new QMediaPlayer;

    void syncSoldierPoolWithFireteam(const std::vector<Actor *> &fireteam, QListWidget *fireteamList);
    void syncFireteamWithSoldierPool(const std::vector<Actor *> &fireteam, const QListWidget *fireteamList);

    void fireteamButtonClicked(std::vector<Actor*> &fireteam, QListWidget *fireteamList);

    void unassignButtonClicked(std::vector<Actor*> &fireteam, QListWidget *fireteamList);

    void updateTeamButton(std::vector<Actor*> &fireteam, QPushButton *teamButton);

    void updateUnassignButton();

    void setActorsKit(const Actor &actor);

    void selectActorsKit();

    std::vector<Kit>& getSelectedActorsKits();

    void buildKitPool(const std::vector<Kit> &kits);

    void updateKitNameBox();

    void updateSelectedKitInfo(const std::vector<Kit> &kits);

    void updateSoldierDetails();

    void updateApplyButton();
};

#endif // PLATOONSETUP_H
