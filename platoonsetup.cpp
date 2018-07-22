#include "platoonsetup.h"
#include "ui_platoonsetup.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <QMessageBox>
#include <QTextStream> // for printing to console

#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "kit.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "strings.h"
namespace fs = std::experimental::filesystem;

static std::vector<Actor> rifleman;
static std::vector<Actor> heavyWeapons;
static std::vector<Actor> sniper;
static std::vector<Actor> demolitions;
static std::vector<Actor> actors;
static std::vector<Actor*> alpha;
static std::vector<Actor*> bravo;
static std::vector<Actor*> charlie;
static uiLists lastSelection{uiLists::NONE};
static std::vector<Kit> kits;
static std::vector<Gun> guns;
static std::vector<Projectile> projectiles;
static std::vector<Item> items;
static Strings strings;

// for debugging
void printActorVector(const std::vector<Actor*> &vec)
{
    for (auto &element : vec)
    {
        //element->print();
        QTextStream(stdout) << element->getName() << endl;
    }
}

/*
// takes a QString and returns the file extension
QString getFileExtension(const QString &fileName)
{
    // starting at the end of the string, check each character
    for (int i{fileName.size() - 1}; i > 0; --i)
    {
        // found period closest to end, now read and return extension
        if (fileName[i] == '.')
        {
            QString extension{""};
            for (int j{i}; j < fileName.size(); ++j)
            {
                extension += fileName[j];
            }
            return extension;
        }
    }
    // no extension found
    QString errorMessage{"Error in getFileExtension().  No extension could be found for filename: "};
    errorMessage += fileName;
    QMessageBox msgBox(QMessageBox::Critical, "Error", errorMessage);
    msgBox.exec();
    return "error";
}
*/


PlatoonSetup::PlatoonSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatoonSetup)
{
    ui->setupUi(this);
    PlatoonSetup::grabKeyboard(); // send all keboard input to the main window to prevent messing up the selection logic of the fireteam/soldier pool boxes

    // set all line edit boxes and plain text boxes to read only
    ui->leName1->setReadOnly(true);
    ui->leWeaponType1->setReadOnly(true);
    ui->leMagCap1->setReadOnly(true);
    ui->leMaxRange1->setReadOnly(true);
    ui->leVelocity1->setReadOnly(true);
    ui->leRecoil1->setReadOnly(true);
    ui->leAccuracy1->setReadOnly(true);
    ui->leStabilization1->setReadOnly(true);
    ui->leMaxZoom1->setReadOnly(true);
    ui->leSilenced1->setReadOnly(true);
    ui->pteFireModes1->setReadOnly(true);
    ui->pteFireModes1->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    ui->leName2->setReadOnly(true);
    ui->leWeaponType2->setReadOnly(true);
    ui->leMagCap2->setReadOnly(true);
    ui->leMaxRange2->setReadOnly(true);
    ui->leVelocity2->setReadOnly(true);
    ui->leRecoil2->setReadOnly(true);
    ui->leAccuracy2->setReadOnly(true);
    ui->leStabilization2->setReadOnly(true);
    ui->leMaxZoom2->setReadOnly(true);
    ui->leSilenced2->setReadOnly(true);
    ui->pteFireModes2->setReadOnly(true);
    ui->pteFireModes2->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
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

    //fs::directory_iterator dirIt ("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman");

    /*
    // read in actors
    for (const auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        actors.push_back(Actor(curFileName, currentFile));
        currentFile.close();
    }
    */

    // read in all actors
    readInActors("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman", rifleman);
    readInActors("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\heavy-weapons", heavyWeapons);
    readInActors("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\sniper", sniper);
    readInActors("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\demolitions", demolitions);
    //readInActors("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\TestMod1\\Actor\\rifleman", actors);

    // randomly choose nine actors of each class and put them into the actors pool
    for (int i{0}; i < 9; ++i) { assignRandomActorToVector(rifleman, actors); }
    for (int i{0}; i < 9; ++i) { assignRandomActorToVector(heavyWeapons, actors); }
    for (int i{0}; i < 9; ++i) { assignRandomActorToVector(sniper, actors); }
    for (int i{0}; i < 9; ++i) { assignRandomActorToVector(demolitions, actors); }


    // build solider pool from actors
    for (const auto &element : actors)
    {
        //element.print();
        new QListWidgetItem(element.getFirstInitialLastName() + "       " + element.getClassName().toUpper(), ui->lwSoldierPool);
    }

    // read in strings
    currentFile.open("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Data\\Shell\\strings.txt");
    strings.readFromFile(currentFile);
    currentFile.close();

    //QString temp = strings.getString("ITM_BOMB2");
    //QTextStream(stdout) << temp << endl;
    // print strings
    /*
    for (const auto &element : strings)
    {
        //QTextStream(stdout) << element. << endl;
    }
    */

    // read in guns
    QString gunExtension{".gun"};
    for (const auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Equip"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        //getFileExtension(curFileName);
        if (QString::compare(getFileExtension(curFileName), gunExtension, Qt::CaseInsensitive) == 0)
        {
            guns.push_back(Gun(curFileName, currentFile, strings));
        }
        currentFile.close();
    }

    // print guns
    for (const auto &element : guns)
    {
        //element.print();
    }

    // read in projectiles
    QString projectileExtension{".prj"};
    for (const auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Equip"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        if (QString::compare(getFileExtension(curFileName), projectileExtension, Qt::CaseInsensitive) == 0)
        {
            projectiles.push_back(Projectile(curFileName, currentFile, strings));
        }
        currentFile.close();
    }

    // print projectiles
    for (const auto &element : projectiles)
    {
        //element.print();
    }

    // read in items
    QString itemExtension{".itm"};
    for (const auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Equip"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        if (QString::compare(getFileExtension(curFileName), itemExtension, Qt::CaseInsensitive) == 0)
        {
            items.push_back(Item(curFileName, currentFile, strings));
        }
        currentFile.close();
    }

    // print items
    for (const auto &element : items)
    {
        //element.print();
    }

    // read in kits
    //dirIt = "C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Kits\\rifleman";
    for (const auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Kits\\rifleman"))
    {
        currentFile.open(element.path());
        QString curFileName{QString::fromStdWString(element.path().filename())};
        kits.push_back(Kit(curFileName, currentFile));
        currentFile.close();
    }

    // build kit pool from kits
    QString item1{""};
    QString item2{""};
    for (const auto &element : kits)
    {

        // the first item in a kit is always a gun
        item1 = element.getSlot1FileName();
        for (const auto &element2 : guns)
        {
            if (QString::compare(item1, element2.getFileName(), Qt::CaseInsensitive) == 0)
            {
                item1 = element2.getName();
            }
        }
        // second item is also a gun
        if (element.getKitType() == element.kitType::TWOGUNS)
        {
            item2 = element.getSlot2FileName();
            for (const auto &element2 : guns)
            {
                if (QString::compare(item2, element2.getFileName(), Qt::CaseInsensitive) == 0)
                {
                    item2 = element2.getName();
                }
            }
        }
        // second item is a projectile/throwable
        else if (element.getKitType() == element.kitType::GUNANDTHROWABLE)
        {
            item2 = element.getSlot2FileName();
            for (const auto &element2 : projectiles)
            {
                if (QString::compare(item2, element2.getFileName(), Qt::CaseInsensitive) == 0)
                {
                    item2 = element2.getName();
                }
            }
        }
        // second item is an item/handheld
        else if (element.getKitType() == element.kitType::GUNANDHANDHELD)
        {
            item2 = element.getSlot2FileName();
            for (const auto &element2 : items)
            {
                if (QString::compare(item2, element2.getFileName(), Qt::CaseInsensitive) == 0)
                {
                    item2 = element2.getName();
                }
            }
        }
        // no second item - extra ammo for slot1's gun
        else if (element.getKitType() == element.kitType::GUNANDAMMO)
        {
            item2 = strings.getString("WPN_EXTRAAMMO");
        }
        // nothing else matched - must be an error
        else
        {
            item2 = "error resolving item2 while building kit list";
        }
        //QString label{item1 + " + " item2};
        item1 += " + ";
        item1 += item2; // add item2 to item1 to create the final label for the list widget
        new QListWidgetItem(item1, ui->lwKits);
    }

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

void PlatoonSetup::on_lwKits_itemClicked()
{
    QString kitNumber{QString::number(ui->lwKits->currentRow() + 1)};
    ui->leKitName->setText(kitNumber);

    updateSelectedKitInfo();
}

void PlatoonSetup::on_pbKitLeft_clicked()
{
    int curRow{ui->lwKits->currentRow()};
    int nextRow{0};
    // if this is the first kit select the last kit
    if (curRow == 0)
    {
        nextRow = ui->lwKits->count() - 1;
    }
    // otherwise select the next kit to the left(minus)
    else
    {
        nextRow = curRow - 1;
    }
    ui->lwKits->setCurrentRow(nextRow);
    updateSelectedKitInfo();
}

void PlatoonSetup::on_pbKitRight_clicked()
{
    int curRow{ui->lwKits->currentRow()};
    int nextRow{0};
    // if this is the last kit select the first kit
    if (curRow == ui->lwKits->count() - 1)
    {
        nextRow = 0;
    }
    // otherwise select the next kit to the right(plus)
    else
    {
        nextRow = curRow + 1;
    }
    ui->lwKits->setCurrentRow(nextRow);
    updateSelectedKitInfo();
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
            QTextStream(stdout) << "found " << actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << " at row " << fireteamList->currentRow() << endl;
            fireteamList->setCurrentRow(i);
            done = true;
            noMatch = false;
            /*
            if (fireteamList->currentRow() != i)
            {
                QTextStream(stdout) << "found " << actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << " at row " << fireteamList->currentRow() << endl;
                fireteamList->setCurrentRow(i);
                done = true;
                noMatch = false;
            }
            else if (fireteamList->currentRow() == i)
            {
                QTextStream(stdout) << "already in sync. found " << actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << " at row " << fireteamList->currentRow() << endl;
                done = true;
                noMatch = false;
            }
            */
        }
    }
    if (noMatch)
    {
        QTextStream(stdout) << "didn't find " << actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << endl;
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

// moves a selected actor to the specified fireteam or promotes an actor already on the team
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
            new QListWidgetItem(element->getFirstInitialLastName(), fireteamList);
        }
    }

    // debug info
    //QTextStream(stdout) << "new backend" << endl;
    //printActorVector(fireteam);
    //QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
}

// removes an actor from the specified fireteam
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
            new QListWidgetItem(element->getFirstInitialLastName(), fireteamList);
        }
    }

    // debug info
    /*
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(fireteam);
    QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
    */
}

// enables / disables the the specfied fireteam button based on the selected actor
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

// enables / disables the unassign button based on the selected actor
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

// updates all the kit detail boxs (mag cap, range, etc) to the currently selected kit when called
void PlatoonSetup::updateSelectedKitInfo()
{
    // first clear all the boxes of any info
    ui->leName1->clear();
    ui->leWeaponType1->clear();
    ui->leMagCap1->clear();
    ui->leMaxRange1->clear();
    ui->leVelocity1->clear();
    ui->leRecoil1->clear();
    ui->leAccuracy1->clear();
    ui->leStabilization1->clear();
    ui->leMaxZoom1->clear();
    ui->leSilenced1->clear();
    ui->pteFireModes1->clear();

    ui->leName2->clear();
    ui->leWeaponType2->clear();
    ui->leMagCap2->clear();
    ui->leMaxRange2->clear();
    ui->leVelocity2->clear();
    ui->leRecoil2->clear();
    ui->leAccuracy2->clear();
    ui->leStabilization2->clear();
    ui->leMaxZoom2->clear();
    ui->leSilenced2->clear();
    ui->pteFireModes2->clear();

    const Kit &currentKit{kits[ui->lwKits->currentRow()]};

    // the first item is always a gun
    const Gun *gun{nullptr};
    for (const auto &element : guns)
    {
        if (QString::compare(currentKit.getSlot1FileName(), element.getFileName(), Qt::CaseInsensitive) == 0)
        {
            gun = &element;
        }
    }

    if (!gun)
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in updateSelectedKitInfo().  Unable to find first item of selected kit.");
        msgBox.exec();
        //QCoreApplication::exit(-1);
        exit(EXIT_FAILURE);
    }

    // get total ammo / number of magazines and display it along with the gun name
    QString magCountText{currentKit.getSlot1MagCount()};
    double magCountNumber{magCountText.toDouble()};
    double magCapNumber{gun->getMagCap().toDouble()};
    double totalRoundsNumber{magCountNumber * magCapNumber};
    QString totalRoundsText{QString::number(totalRoundsNumber)};
    ui->leName1->setText(gun->getName() + "   " + totalRoundsText + "/" + magCountText);

    ui->leMagCap1->setText(gun->getMagCap());
    ui->leMaxRange1->setText(gun->getMaxRange() + " m");
    ui->leVelocity1->setText(gun->getMuzzleVelocity() + " m/s");
    ui->leRecoil1->setText(gun->getRecoil());
    ui->leAccuracy1->setText(gun->getMaxAccuracy());
    ui->leStabilization1->setText(gun->getStabilizationTime() + " s");
    QString weaponType{gun->getWeaponType()};
    QString weaponTypeText{"Unspecified Type"};
    if (weaponType == "0")
        weaponTypeText = "Pistol";
    else if (weaponType == "1")
        weaponTypeText = "Rifle";
    else if (weaponType == "2")
        weaponTypeText = "Sniper Rifle";
    else if (weaponType == "3")
        weaponTypeText = "Bolt Action";
    else if (weaponType == "4")
        weaponTypeText = "Grenade Launcher";
    else if (weaponType == "5")
        weaponTypeText = "Rocket Launcher";
    else if (weaponType == "6")
        weaponTypeText = "Shotgun";
    ui->leWeaponType1->setText(weaponTypeText);
    if (gun->getSilenced() == "1")
    {
        ui->leSilenced1->setText("Yes");
    }
    else
    {
        ui->leSilenced1->setText("No");
    }
    ui->leMaxZoom1->setText(gun->getMaxZoom() + "x");
    QString fireModesForBox{""};
    for (auto &element : gun->getFireModes())
    {
        fireModesForBox += element.rpm;
        fireModesForBox += " rpm ";
        if (element.mode == "Full Auto")
        {
            //fireModesForBox += " ";
            fireModesForBox += "Full Auto";
        }
        else if (element.mode == "1")
        {
            //fireModesForBox += " ";
            fireModesForBox += "Single Shot";
        }
        else
        {
            fireModesForBox += element.mode;
            fireModesForBox += " Round Burst";
        }
        fireModesForBox += '\n';
    }
    ui->pteFireModes1->setPlainText(fireModesForBox);

    // second item is also a gun
    if (currentKit.getKitType() == Kit::kitType::TWOGUNS)
    {
        const Gun *gun2{nullptr};
        for (const auto &element : guns)
        {
            if (QString::compare(currentKit.getSlot2FileName(), element.getFileName(), Qt::CaseInsensitive) == 0)
            {
                gun2 = &element;
            }
        }

        if (!gun2)
        {
            QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in updateSelectedKitInfo().  Unable to find second item of selected kit.");
            msgBox.exec();
            //QCoreApplication::exit(-1);
            exit(EXIT_FAILURE);

        }

        // get total ammo / number of magazines and display it along with the gun name
        magCountText = currentKit.getSlot2MagCount();
        magCountNumber = magCountText.toDouble();
        magCapNumber = gun2->getMagCap().toDouble();
        totalRoundsNumber = magCountNumber * magCapNumber;
        totalRoundsText = QString::number(totalRoundsNumber);
        ui->leName2->setText(gun2->getName() + "   " + totalRoundsText + "/" + magCountText);

        ui->leMagCap2->setText(gun2->getMagCap());
        ui->leMaxRange2->setText(gun2->getMaxRange() + " m");
        ui->leVelocity2->setText(gun2->getMuzzleVelocity() + " m/s");
        ui->leRecoil2->setText(gun2->getRecoil());
        ui->leAccuracy2->setText(gun2->getMaxAccuracy());
        ui->leStabilization2->setText(gun2->getStabilizationTime() + " s");
        weaponType = gun2->getWeaponType();
        weaponTypeText = "Unspecified Type";
        if (weaponType == "0")
            weaponTypeText = "Pistol";
        else if (weaponType == "1")
            weaponTypeText = "Rifle";
        else if (weaponType == "2")
            weaponTypeText = "Sniper Rifle";
        else if (weaponType == "3")
            weaponTypeText = "Bolt Action";
        else if (weaponType == "4")
            weaponTypeText = "Grenade Launcher";
        else if (weaponType == "5")
            weaponTypeText = "Rocket Launcher";
        else if (weaponType == "6")
            weaponTypeText = "Shotgun";
        ui->leWeaponType2->setText(weaponTypeText);
        if (gun2->getSilenced() == "1")
        {
            ui->leSilenced2->setText("Yes");
        }
        else
        {
            ui->leSilenced2->setText("No");
        }
        ui->leMaxZoom2->setText(gun2->getMaxZoom() + "x");
        fireModesForBox = "";
        for (auto &element : gun2->getFireModes())
        {
            fireModesForBox += element.rpm;
            fireModesForBox += " rpm ";
            if (element.mode == "Full Auto")
            {
                fireModesForBox += "Full Auto";
            }
            else if (element.mode == "1")
            {
                fireModesForBox += "Single Shot";
            }
            else
            {
                fireModesForBox += element.mode;
                fireModesForBox += " Round Burst";
            }
            fireModesForBox += '\n';
        }
        ui->pteFireModes2->setPlainText(fireModesForBox);
    }

    // second item is a projectile/throwable
    if (currentKit.getKitType() == Kit::kitType::GUNANDTHROWABLE)
    {
        const Projectile *projectile{nullptr};
        for (const auto &element : projectiles)
        {
            if (QString::compare(currentKit.getSlot2FileName(), element.getFileName(), Qt::CaseInsensitive) == 0)
            {
                projectile = &element;
            }
        }

        if (!projectile)
        {
            QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in updateSelectedKitInfo().  Unable to find second item of selected kit.");
            msgBox.exec();
            //QCoreApplication::exit(-1);
            exit(EXIT_FAILURE);
        }

        // get quantity of this thowable and display it along with the throwable's name
        QString itemCount{currentKit.getSlot2ItemCount()};
        ui->leName2->setText(projectile->getName() + "  x" + itemCount);
    }

    // second item is an item/handheld
    if (currentKit.getKitType() == Kit::kitType::GUNANDHANDHELD)
    {
        const Item *item{nullptr};
        for (const auto &element : items)
        {
            if (QString::compare(currentKit.getSlot2FileName(), element.getFileName(), Qt::CaseInsensitive) == 0)
            {
                item = &element;
            }
        }

        if (!item)
        {
            QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in updateSelectedKitInfo().  Unable to find second item of selected kit.");
            msgBox.exec();
            //QCoreApplication::exit(-1);
            exit(EXIT_FAILURE);
        }

        // get quantity of this handheld and display it along with the handheld's name
        QString itemCount{currentKit.getSlot2ItemCount()};
        ui->leName2->setText(item->getName() + "  x" + itemCount);
    }

    // no second item - extra ammo for first item
    if (currentKit.getKitType() == Kit::kitType::GUNANDAMMO)
    {
        // remove extra ammo from item1's count
        double extraAmmoNumber{currentKit.getExtraAmmo().toDouble()};
        magCountNumber -= extraAmmoNumber;
        magCountText = QString::number(magCountNumber);
        totalRoundsNumber = magCountNumber * magCapNumber;
        totalRoundsText = QString::number(totalRoundsNumber);
        ui->leName1->setText(gun->getName() + "   " + totalRoundsText + "/" + magCountText);

        // get total ammo / number of magazines and display it next to extra ammo's name
        totalRoundsNumber = extraAmmoNumber * magCapNumber;
        totalRoundsText = QString::number(totalRoundsNumber);
        ui->leName2->setText(strings.getString("WPN_EXTRAAMMO") + "   " + totalRoundsText + "/" + currentKit.getExtraAmmo());
    }
}
