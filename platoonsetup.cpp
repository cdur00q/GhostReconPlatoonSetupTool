#include "platoonsetup.h"
#include "ui_platoonsetup.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <QTextStream> // for printing to console

#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "kit.h"
#include "gun.h"
namespace fs = std::experimental::filesystem;

static std::vector<Actor> actors;
static std::vector<Actor*> alpha;
static std::vector<Actor*> bravo;
static std::vector<Actor*> charlie;
static uiLists lastSelection{uiLists::NONE};
static std::vector<Kit> kits;

// for debugging
void printActorVector(const std::vector<Actor*> &vec)
{
    for (auto &element : vec)
    {
        //element->print();
        QTextStream(stdout) << element->getName() << endl;
    }
}


PlatoonSetup::PlatoonSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatoonSetup)
{
    ui->setupUi(this);
    PlatoonSetup::grabKeyboard(); // send all keboard input to the main window to prevent messing up the selection logic of the fireteam/soldier pool boxes

    // model / view test
    /*
    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman");
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"));
    */

    //QTextStream(stdout) << "string to print" << endl;



    //fs::path b(a->path());

    actors.reserve(76); // 76 rifleman, mods could have less or more?

    std::ifstream currentFile;
    /*
    currentFile.open("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman\\rifleman-01.atr");
    actor actor1(currentFile);
    actor1.print();
    */

    fs::directory_iterator dirIt ("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman");

    //int count{0};
    // read in actors
    for (auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"))
    {
        //fs::path temp(element.path());
        //auto temp2{temp.filename()};
        //QString temp3{QString::fromStdWString(temp.filename())};
        currentFile.open(element.path());
        //actor actor(currentFile);
        QString curFileName{QString::fromStdWString(element.path().filename())};
        actors.push_back(Actor(curFileName, currentFile));
        currentFile.close();


        //QTextStream(stdout) << element << endl;
        //++count;
    }

    // build solider pool from actors
    for (auto &element : actors)
    {
        //element.print();
        new QListWidgetItem(element.getName(), ui->lwSoldierPool);
    }

    // read in kits
    //dirIt = "C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Kits\\rifleman";
    for (auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Kits\\rifleman"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        kits.push_back(Kit(curFileName, currentFile));
        currentFile.close();
    }

    // build kit pool from kits
    for (auto &element : kits)
    {
        //element.print();
        //new QListWidgetItem(element.getName(), ui->lwSoldierPool);
    }

    // read in guns
    currentFile.open("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Equip\\m16.GUN");
    Gun("m16.GUN", currentFile);

    /*
    std::string fileName{"C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman\\rifleman-01.atr"};
    std::ifstream actorFile(fileName);
    if (actorFile.good())
        QTextStream(stdout) << "good" << '\n';
    else
        QTextStream(stdout) << "bad" << '\n';
    */

    // writing avatar file
    std::string coopAvatarPath{"C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Data\\Temp\\coop_avatar.toe"};
    std::ofstream coopAvatar(coopAvatarPath, std::ios::out|std::ios::trunc);
    writeCoopAvatar(coopAvatar);
}

PlatoonSetup::~PlatoonSetup()
{
    delete ui;
}

void PlatoonSetup::on_pbAlpha_clicked()
{
    unassignButtonClicked(bravo, ui->lwBravo);
    unassignButtonClicked(charlie, ui->lwCharlie);
    fireteamButtonClicked(alpha, ui->lwAlpha);
    syncSoldierPoolWithFireteam(alpha, ui->lwAlpha);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
}

void PlatoonSetup::on_pbBravo_clicked()
{
    unassignButtonClicked(alpha, ui->lwAlpha);
    unassignButtonClicked(charlie, ui->lwCharlie);
    fireteamButtonClicked(bravo, ui->lwBravo);
    syncSoldierPoolWithFireteam(bravo, ui->lwBravo);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
}

void PlatoonSetup::on_pbCharlie_clicked()
{
    unassignButtonClicked(alpha, ui->lwAlpha);
    unassignButtonClicked(bravo, ui->lwBravo);
    fireteamButtonClicked(charlie, ui->lwCharlie);
    syncSoldierPoolWithFireteam(charlie, ui->lwCharlie);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
}

void PlatoonSetup::on_lwSoldierPool_itemClicked()
{
    syncSoldierPoolWithFireteam(alpha, ui->lwAlpha);
    syncSoldierPoolWithFireteam(bravo, ui->lwBravo);
    syncSoldierPoolWithFireteam(charlie, ui->lwCharlie);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
}

void PlatoonSetup::on_lwAlpha_itemClicked()
{
    syncFireteamWithSoldierPool(alpha, ui->lwAlpha);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwBravo->clearSelection();
    ui->lwCharlie->clearSelection();
}

void PlatoonSetup::on_lwBravo_itemClicked()
{
    syncFireteamWithSoldierPool(bravo, ui->lwBravo);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwAlpha->clearSelection();
    ui->lwCharlie->clearSelection();
}

void PlatoonSetup::on_lwCharlie_itemClicked()
{
    syncFireteamWithSoldierPool(charlie, ui->lwCharlie);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwAlpha->clearSelection();
    ui->lwBravo->clearSelection();
}

void PlatoonSetup::on_pbUnassign_clicked()
{
    unassignButtonClicked(alpha, ui->lwAlpha);
    unassignButtonClicked(bravo, ui->lwBravo);
    unassignButtonClicked(charlie, ui->lwCharlie);
    updateTeamButton(alpha, ui->pbAlpha);
    updateTeamButton(bravo, ui->pbBravo);
    updateTeamButton(charlie, ui->pbCharlie);
    updateUnassignButton();
}

// sync a selection from the soldier pool with the specified fireteam
// check if selected soldier is on any of the specified team and select them there if so
void PlatoonSetup::syncSoldierPoolWithFireteam(const std::vector<Actor*> &fireteam, QListWidget *fireteamList)
{
    bool done{false};
    bool noMatch{true};
    // for each member of fireteam
    for (int i{0}; i < fireteam.size() && !done; ++i)
    {
        // compare if they are the same actor that is selected in soldier pool
        if (actors[ui->lwSoldierPool->currentRow()] == *fireteam[i])
        {
            // they are, now check if the current rows are in sync
            QTextStream(stdout) << "found " << actors[ui->lwSoldierPool->currentRow()].getName() << " at row " << fireteamList->currentRow() << endl;
            fireteamList->setCurrentRow(i);
            done = true;
            noMatch = false;
            /*
            if (fireteamList->currentRow() != i)
            {
                QTextStream(stdout) << "found " << actors[ui->lwSoldierPool->currentRow()].getName() << " at row " << fireteamList->currentRow() << endl;
                fireteamList->setCurrentRow(i);
                done = true;
                noMatch = false;
            }
            else if (fireteamList->currentRow() == i)
            {
                QTextStream(stdout) << "already in sync. found " << actors[ui->lwSoldierPool->currentRow()].getName() << " at row " << fireteamList->currentRow() << endl;
                done = true;
                noMatch = false;
            }
            */
        }
    }
    if (noMatch)
    {
        QTextStream(stdout) << "didn't find " << actors[ui->lwSoldierPool->currentRow()].getName() << endl;
        fireteamList->clearSelection();
    }
}

// sync a selection from the specified fireteam with the soldier pool
// check if selected soldier is on any of the specified team and select them there if so
void PlatoonSetup::syncFireteamWithSoldierPool(const std::vector<Actor*> &fireteam, const QListWidget *fireteamList)
{
    bool done{false};
    bool noMatch{true};
    // for each member of soldier pool
    for (int i{0}; i < actors.size() && !done; ++i)
    {
        // compare if they are the same actor that is selected in the fireteam
        if (actors[i] == *fireteam[fireteamList->currentRow()])
        {
            ui->lwSoldierPool->setCurrentRow(i);
            done = true;
            noMatch = false;
        }
    }
    if (noMatch)
    {
        ui->lwSoldierPool->clearSelection();
    }
}

void PlatoonSetup::fireteamButtonClicked(std::vector<Actor *> &fireteam, QListWidget *fireteamList)
{
    int selectedActor{ui->lwSoldierPool->currentRow()};

    // check if actor is on fireteam already
    bool onTeam{false};
    for (auto &element : fireteam)
    {
        if (actors[selectedActor] == *element)
        onTeam = true;
    }

    // actor not on team already
    if (!onTeam)
    fireteam.push_back(&actors[selectedActor]);

    // actor on team already so this is a promotion
    else if (onTeam && fireteam.size() >= 2)
    {
        // find actor's place on team
        bool done{false};
        for (int i{0}; i < fireteam.size() && !done; ++i)
        {
            // found actor
            if (*fireteam[i] == actors[selectedActor])
            {
                // actor is first, so move to back
                if (i == 0)
                {
                    fireteam.push_back(fireteam[0]);
                    // and then move everyone else up
                    for (int j{i}; j < fireteam.size() - 1; ++j)
                    {
                        fireteam[j] = fireteam[j + 1];
                    }
                    fireteam.pop_back();
                    done = true;
                }

                // actor is not first
                if (i > 0)
                {
                    // swap with actor before this actor(promotion)
                    Actor* temp{fireteam[i - 1]};
                    fireteam[i - 1] = fireteam[i];
                    fireteam[i] = temp;
                }
            }
        }
    }

    // backend done, now update front end
    // delete all current entries
    while (fireteamList->count() > 0)
    {
        delete fireteamList->takeItem(0);
        QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
    }

    // and recreate list based on new backend
    if (fireteam.size() > 0)
    {
        for (const auto &element : fireteam)
        {
            new QListWidgetItem(element->getName(), fireteamList);
        }
    }

    // debug info
    //QTextStream(stdout) << "new backend" << endl;
    //printActorVector(fireteam);
    //QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
}

void PlatoonSetup::unassignButtonClicked(std::vector<Actor*> &fireteam, QListWidget *fireteamList)
{
    int selectedActor{ui->lwSoldierPool->currentRow()};

    // check if actor is on specified team already
    bool onTeam{false};
    for (auto &element : fireteam)
    {
        if (actors[selectedActor] == *element)
        onTeam = true;
    }

    // actor on specified team so remove actor now
    if (onTeam)
    {
        // find actor's place on team
        for (int i{0}; i < fireteam.size(); ++i)
        {
            // found actor
            if (*fireteam[i] == actors[selectedActor])
            {
                // actor is last so just remove
                if (i == fireteam.size() - 1)
                {
                    fireteam.pop_back();
                    //QTextStream(stdout) << "actor last" << endl;
                    //printActorVector(fireteam);
                }
                // actor not last
                else
                {
                    // schuffle everyone up one
                    for (int j{i}; j < fireteam.size() - 1; ++j)
                    {
                        fireteam[j] = fireteam[j + 1];
                    }
                    fireteam.pop_back(); // and remove last
                    //QTextStream(stdout) << "actor not last" << endl;
                    //printActorVector(fireteam);
                }
            }
        }
    }

    // backend done, now update front end
    // delete all current entries
    while (fireteamList->count() > 0)
    {
        delete fireteamList->takeItem(0);
    }

    // and recreate list based on new backend
    if (fireteam.size() > 0)
    {
        for (const auto &element : fireteam)
        {
            new QListWidgetItem(element->getName(), fireteamList);
        }
    }

    // debug info
    /*
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(fireteam);
    QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
    */
}

void PlatoonSetup::updateTeamButton(std::vector<Actor*> &fireteam, QPushButton *teamButton)
{
    // check if actor is on specified team already
    bool onTeam{false};
    for (auto &element : fireteam)
    {
        if (actors[ui->lwSoldierPool->currentRow()] == *element)
        onTeam = true;
    }

    // it is by itself
    if (fireteam.size() == 1 && onTeam == true)
    {
        teamButton->setEnabled(false);
    }

    // it is with 1 or more other actors
    else if (fireteam.size() > 1 && onTeam == true)
    {
        teamButton->setEnabled(true);
    }

    // it isn't but there is room
    else if (fireteam.size() < fireteamMaxMembers && onTeam == false)
    {
        teamButton->setEnabled(true);
    }

    // it isn't and no room on team
    else if (fireteam.size() >= fireteamMaxMembers && onTeam == false)
    {
        teamButton->setEnabled(false);
    }
}

void PlatoonSetup::updateUnassignButton()
{
    bool onAnyTeam{false};

    // check if actor is on alpha team
    for (auto &element : alpha)
    {
        if (actors[ui->lwSoldierPool->currentRow()] == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // check if actor is on bravo team
    for (auto &element : bravo)
    {
        if (actors[ui->lwSoldierPool->currentRow()] == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // check if actor is on charlie team
    for (auto &element : charlie)
    {
        if (actors[ui->lwSoldierPool->currentRow()] == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // not on any of the teams
    if (!onAnyTeam)
        ui->pbUnassign->setEnabled(false);
}
