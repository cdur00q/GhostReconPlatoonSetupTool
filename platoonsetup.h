#ifndef PLATOONSETUP_H
#define PLATOONSETUP_H

#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <vector>
#include <memory> // for std::unique_ptr and std::make_unique

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

    void on_lwSoldierPool_itemPressed();

    void on_lwAlpha_itemPressed();

    void on_lwBravo_itemPressed();

    void on_lwCharlie_itemPressed();

    void on_pbUnassign_clicked();

    void on_lwKits_itemPressed();

    void on_pbKitLeft_clicked();

    void on_pbKitRight_clicked();

    void on_pbKitFireteam_clicked();

    void on_pbKitSquad_clicked();

    void on_pbApply_clicked();

    void mediaPlayerStateChanged(QMediaPlayer::State state);

    void qApplicationStateChanged(Qt::ApplicationState state);

private:
    Ui::PlatoonSetup *ui;

    std::vector<Actor> m_actors;
    std::vector<Actor*> m_alpha; // these point to elements in actors, which lasts until the end of the program so no need to worry about dangling pointers
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
    QMediaPlayer *m_mediaPlayer = new QMediaPlayer; // can't use a std::unique_ptr here because the signal/slot connect method requires raw pointers
    std::unique_ptr<QMediaPlayer> m_mediaPlayerButtonClick = std::make_unique<QMediaPlayer>(nullptr);
    std::unique_ptr<QMediaPlayer> m_mediaPlayerApplyClick = std::make_unique<QMediaPlayer>(nullptr);

    // sync a selection from the soldier pool with the specified fireteam
    // check if selected soldier is on the specified team and select them there if so
    void syncSoldierPoolWithFireteam(const std::vector<Actor *> &fireteam, QListWidget *fireteamList);

    // sync a selection from the specified fireteam with the soldier pool
    void syncFireteamWithSoldierPool(const std::vector<Actor *> &fireteam, const QListWidget *fireteamList);

    // moves a selected actor to the specified fireteam or promotes an actor already on the team
    void fireteamButtonClicked(std::vector<Actor*> &fireteam, QListWidget *fireteamList);

    // removes an actor from the specified fireteam
    void unassignButtonClicked(std::vector<Actor*> &fireteam, QListWidget *fireteamList);

    // enables / disables the the specfied fireteam button based on the selected actor
    void updateTeamButton(std::vector<Actor*> &fireteam, QPushButton *teamButton);

    // enables / disables the unassign button based on the selected actor
    void updateUnassignButton();

    // assigns the selected kit to the passed in actor
    void setActorsKit(const Actor &actor);

    // selects actor's current kit from the kit list
    void selectActorsKit();

    // returns a reference to the appropriate kit vector of the selected actor
    std::vector<Kit>& getSelectedActorsKits();

    // builds kit pool from passed in kit vector
    void buildKitPool(const std::vector<Kit> &kits);

    // updates the box that displays the currently selected kit's name
    void updateKitNameBox();

    // updates all the kit detail boxes (mag cap, range, etc) to the currently selected kit
    void updateSelectedKitInfo(const std::vector<Kit> &kits);

    // updates the big soldier name box as well as soldier stats to the selected soldier
    void updateSoldierDetails();

    // enables / disables the apply kit to fireteam button
    void updateApplyKitToFireteamButton();

    // enables / disables the apply kit to squad button
    void updateApplyKitToSquadButton();

    // enables / disables the apply button
    void updateApplyButton();
};

#endif // PLATOONSETUP_H
