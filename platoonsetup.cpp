#include "platoonsetup.h"
#include "ui_platoonsetup.h"

//#include <QFileSystemModel>
//#include <QTreeView>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <map>
#include <utility> // for std::make_pair
#include <QMessageBox>
#include <QtMultimedia/QMediaPlayer>
//#include <QUrl>
//#include <QtMultimedia/QMediaContent>
#include <QTextStream> // for printing to console

#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "kit.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "strings.h"
#include "kitrestrictionlist.h"
#include "assignedkitmap.h"
#include "modlist.h"
namespace fs = std::experimental::filesystem;

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
    ui->pteFireModes1->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    ui->pteFireModes2->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    //QTextStream(stdout) << "string to print" << endl;

    // working with quotes
    // printf(R"(She said "time flies like an arrow, but fruit flies like a banana".)");
    // https://stackoverflow.com/questions/12338818/how-to-get-double-quotes-into-a-string-literal#12338826

    //TODO - interface styling
    //TODO - fix indentation between soldier name and class in soldier pool
    //TODO - create two different versions of wirteAvatar() - one for SP and one for MP
    //TODO - return value of various get game data functions in the classes - switch them to void and call a function on error?
    //TODO - add 'is regular file' check to readingamefiles()?
    //TODO - weapon types for support kits showing "rifle" - any way to fix?
    //TODO - first available soldier is automatically selected when program loads?
    //TODO - unassign all button?

    m_actors.reserve(76); // do this for all the vectors that store actors in the program?

    std::ifstream currentFile;

    // read in all actors
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\rifleman", actorExtension, m_rifleman);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\heavy-weapons", actorExtension, m_heavyWeapons);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\sniper", actorExtension, m_sniper);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\demolitions", actorExtension, m_demolitions);

    // randomly choose nine actors of each class and put them into the actors pool
    if (m_rifleman.size() > 0) for (int i{0}; i < 9; ++i) { assignRandomActorToVector(m_rifleman, m_actors); }
    if (m_heavyWeapons.size() > 0) for (int i{0}; i < 9; ++i) { assignRandomActorToVector(m_heavyWeapons, m_actors); }
    if (m_sniper.size() > 0) for (int i{0}; i < 9; ++i) { assignRandomActorToVector(m_sniper, m_actors); }
    if (m_demolitions.size() > 0) for (int i{0}; i < 9; ++i) { assignRandomActorToVector(m_demolitions, m_actors); }

    // read in strings
    currentFile.open(mainGameDirectory + "\\Data\\Shell\\strings.txt");
    m_strings.readFromFile(currentFile);
    currentFile.close();

    // print strings
    /*
    for (const auto &element : strings)
    {
        //QTextStream(stdout) << element. << endl;
    }
    */

    // read in guns
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", gunExtension, m_guns, m_strings);

    // print guns
    for (const auto &element : m_guns)
    {
        //element.print();
    }

    // read in projectiles
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", projectileExtension, m_projectiles, m_strings);

    // print projectiles
    for (const auto &element : m_projectiles)
    {
        //element.print();
    }

    // read in items
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", itemExtension, m_items, m_strings);

    // print items
    for (const auto &element : m_items)
    {
        //element.print();
    }

    // read in base kits for the four character classes and store them into their respective kit vectors
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\rifleman", kitExtension, m_riflemanKits);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\heavy-weapons", kitExtension, m_heavyWeaponsKits);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\sniper", kitExtension, m_sniperKits);
    readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\demolitions", kitExtension, m_demolitionsKits);

    // read in all the kits and store them into a temporary kit vector
    std::vector<Kit> tempKits;
    readInAllKits(mainGameDirectory + "\\Mods\\Origmiss\\Kits", tempKits);

    // create the kit restriction list
    currentFile.open(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\quick_missions.qmk");
    KitRestrictionList kitList(currentFile);
    currentFile.close();

    updateKitVectorPerRestrictionList(tempKits, kitList, m_riflemanKits, m_heavyWeaponsKits, m_sniperKits, m_demolitionsKits);

    // create music tracks
    std::string musicAction3{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\action3.wav"};
    std::string musicLoad1{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\load1.wav"};
    std::string musicLoad3{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\load3.wav"};

    // read in the mod list
    currentFile.open(mainGameDirectory + "\\modsset.txt");
    ModList modList(currentFile);
    currentFile.close();

    // load mods
    for (const auto &currentMod : modList.getModList())
    {
        loadMod(mainGameDirectory + currentMod, m_actors, m_strings, m_guns, m_projectiles, m_items, m_riflemanKits, m_heavyWeaponsKits, m_sniperKits, m_demolitionsKits, musicAction3, musicLoad1, musicLoad3);
    }

    // build solider pool from actors (must do after loading mods as mods may change actor names)
    for (const auto &element : m_actors)
    {
        //element.print();
        if (element.getClassName() == classDemolitions)
        {
            new QListWidgetItem(element.getFirstInitialLastName() + "       " + "DEMO", ui->lwSoldierPool);
        }
        else
        {
            new QListWidgetItem(element.getFirstInitialLastName() + "       " + element.getClassName().toUpper(), ui->lwSoldierPool);
        }
    }

    // assign default kits to actors.  the default kits are the first kits of all the availble kits
    int defaultKitIndex{0};
    for (auto &element : m_actors)
    {
        if (element.getKitPath() == classRifleman)
        {
            m_assignedKitMap.assignKitToActor(m_riflemanKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getKitPath() == classHeavyWeapons)
        {
            m_assignedKitMap.assignKitToActor(m_heavyWeaponsKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getKitPath() == classSniper)
        {
            m_assignedKitMap.assignKitToActor(m_sniperKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getKitPath() == classDemolitions)
        {
            m_assignedKitMap.assignKitToActor(m_demolitionsKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else
        {
            QString errorMsg{"Error while assigning default kits.  Unrecognized kit path: '"};
            errorMsg += element.getKitPath();
            errorMsg += "' for actor: ";
            errorMsg += element.getFirstInitialLastName();
            errorMsg += " filename: ";
            errorMsg += element.getFileName();
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    }

    // connect signals and slots
    QCoreApplication *coreApp{QCoreApplication::instance()};
    QObject::connect(coreApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(qApplicationStateChanged(Qt::ApplicationState)));
    QObject::connect(m_mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaPlayerStateChanged(QMediaPlayer::State)));

    // start music playback
    switch (getRandomNumber(0, 2))
    {
    case 0:
        m_mediaPlayer->setMedia(stringToQUrl(musicAction3));
        break;
    case 1:
        m_mediaPlayer->setMedia(stringToQUrl(musicLoad1));
        break;
    case 2:
        m_mediaPlayer->setMedia(stringToQUrl(musicLoad3));
        break;
    default:
        QMessageBox msgBox(QMessageBox::Warning, "Warning", "Warning, unhandled case when selecting random music track in PlatoonSetup::PlatoonSetup().");
        msgBox.exec();
    }
    m_mediaPlayer->play();
}

PlatoonSetup::~PlatoonSetup()
{
    delete ui;
    delete m_mediaPlayer;
}

void PlatoonSetup::on_pbAlpha_clicked()
{
    unassignButtonClicked(m_bravo, ui->lwBravo);
    unassignButtonClicked(m_charlie, ui->lwCharlie);
    fireteamButtonClicked(m_alpha, ui->lwAlpha);
    syncSoldierPoolWithFireteam(m_alpha, ui->lwAlpha);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    updateApplyButton();
}

void PlatoonSetup::on_pbBravo_clicked()
{
    unassignButtonClicked(m_alpha, ui->lwAlpha);
    unassignButtonClicked(m_charlie, ui->lwCharlie);
    fireteamButtonClicked(m_bravo, ui->lwBravo);
    syncSoldierPoolWithFireteam(m_bravo, ui->lwBravo);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    updateApplyButton();
}

void PlatoonSetup::on_pbCharlie_clicked()
{
    unassignButtonClicked(m_alpha, ui->lwAlpha);
    unassignButtonClicked(m_bravo, ui->lwBravo);
    fireteamButtonClicked(m_charlie, ui->lwCharlie);
    syncSoldierPoolWithFireteam(m_charlie, ui->lwCharlie);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    updateApplyButton();
}

void PlatoonSetup::on_lwSoldierPool_itemClicked()
{
    syncSoldierPoolWithFireteam(m_alpha, ui->lwAlpha);
    syncSoldierPoolWithFireteam(m_bravo, ui->lwBravo);
    syncSoldierPoolWithFireteam(m_charlie, ui->lwCharlie);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();

    buildKitPool(getSelectedActorsKits());
    selectActorsKit();
    updateKitNameBox();
    updateSelectedKitInfo(getSelectedActorsKits());

    updateSoldierDetails();
}

void PlatoonSetup::on_lwAlpha_itemClicked()
{
    syncFireteamWithSoldierPool(m_alpha, ui->lwAlpha);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwBravo->clearSelection();
    ui->lwCharlie->clearSelection();

    buildKitPool(getSelectedActorsKits());
    selectActorsKit();
    updateKitNameBox();
    updateSelectedKitInfo(getSelectedActorsKits());

    updateSoldierDetails();
}

void PlatoonSetup::on_lwBravo_itemClicked()
{
    syncFireteamWithSoldierPool(m_bravo, ui->lwBravo);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwAlpha->clearSelection();
    ui->lwCharlie->clearSelection();

    buildKitPool(getSelectedActorsKits());
    selectActorsKit();
    updateKitNameBox();
    updateSelectedKitInfo(getSelectedActorsKits());

    updateSoldierDetails();
}

void PlatoonSetup::on_lwCharlie_itemClicked()
{
    syncFireteamWithSoldierPool(m_charlie, ui->lwCharlie);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    ui->lwAlpha->clearSelection();
    ui->lwBravo->clearSelection();

    buildKitPool(getSelectedActorsKits());
    selectActorsKit();
    updateKitNameBox();
    updateSelectedKitInfo(getSelectedActorsKits());

    updateSoldierDetails();
}

void PlatoonSetup::on_pbUnassign_clicked()
{
    unassignButtonClicked(m_alpha, ui->lwAlpha);
    unassignButtonClicked(m_bravo, ui->lwBravo);
    unassignButtonClicked(m_charlie, ui->lwCharlie);
    updateTeamButton(m_alpha, ui->pbAlpha);
    updateTeamButton(m_bravo, ui->pbBravo);
    updateTeamButton(m_charlie, ui->pbCharlie);
    updateUnassignButton();
    updateApplyButton();
}

void PlatoonSetup::on_lwKits_itemClicked()
{
    updateKitNameBox();
    updateSelectedKitInfo(getSelectedActorsKits());
    setActorsKit(m_actors[ui->lwSoldierPool->currentRow()]);
}

void PlatoonSetup::on_pbKitLeft_clicked()
{
    // if there is at least one kit in the kit list
    if (ui->lwKits->count() >= 1){
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

        updateKitNameBox();
        updateSelectedKitInfo(getSelectedActorsKits());
        setActorsKit(m_actors[ui->lwSoldierPool->currentRow()]);
    }
}

void PlatoonSetup::on_pbKitRight_clicked()
{
    // if there is at least one kit in the kit list
    if (ui->lwKits->count() >= 1){
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

        updateKitNameBox();
        updateSelectedKitInfo(getSelectedActorsKits());
        setActorsKit(m_actors[ui->lwSoldierPool->currentRow()]);
    }
}

void PlatoonSetup::on_pbKitFireteam_clicked()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    std::vector<Actor*> *fireteamPtr{nullptr};

    // check if selected actor is on fireteam alpha
    for (const auto &element : m_alpha)
    {
        if (selectedActor == *element) // they are
            fireteamPtr = &m_alpha; // set temporary pointer to alpha
    }

    for (const auto &element : m_bravo)
    {
        if (selectedActor == *element)
            fireteamPtr = &m_bravo;
    }

    for (const auto &element : m_charlie)
    {
        if (selectedActor == *element)
            fireteamPtr = &m_charlie;
    }

    // iterate through whatever fireteam the selected actor is on and assign their kit to any other actors that share the same kit path
    for (const auto &element : *fireteamPtr)
    {
        if (element->getKitPath() == selectedActor.getKitPath() && *element != selectedActor)
            setActorsKit(*element);
    }
}

void PlatoonSetup::on_pbKitSquad_clicked()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};

    // iterate through all three fireteams and assign the selected actor's kit to any other actors that share the same kit path
    for (const auto &element : m_alpha)
    {
        if (element->getKitPath() == selectedActor.getKitPath() && *element != selectedActor)
            setActorsKit(*element);
    }

    for (const auto &element : m_bravo)
    {
        if (element->getKitPath() == selectedActor.getKitPath() && *element != selectedActor)
            setActorsKit(*element);
    }

    for (const auto &element : m_charlie)
    {
        if (element->getKitPath() == selectedActor.getKitPath() && *element != selectedActor)
            setActorsKit(*element);
    }
}

void PlatoonSetup::on_pbApply_clicked()
{
    // write avatar files
    //std::string avatarPath{mainGameDirectory + "\\Data\\Temp\\avatar.toe"};
    //std::string coopAvatarPath{mainGameDirectory + "\\Data\\Temp\\coop_avatar.toe"};
    std::ofstream avatar(mainGameDirectory + "\\Data\\Temp\\avatar.toe", std::ios::out|std::ios::trunc);
    writeAvatarFile(m_alpha, m_bravo, m_charlie, m_assignedKitMap, avatar, false);
    avatar.close();

    std::ofstream coopAvatar(mainGameDirectory + "\\Data\\Temp\\coop_avatar.toe", std::ios::out|std::ios::trunc);
    writeAvatarFile(m_alpha, m_bravo, m_charlie, m_assignedKitMap, coopAvatar, true);
    coopAvatar.close();

    QMessageBox msgBox(QMessageBox::Information, "Info", "Done");
    msgBox.exec();
}

void PlatoonSetup::mediaPlayerStateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
        m_mediaPlayer->play();
}

void PlatoonSetup::qApplicationStateChanged(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationInactive)
    {
        m_mediaPlayer->pause();
    }
    else if (state == Qt::ApplicationActive)
    {
        m_mediaPlayer->play();
    }
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
        if (m_actors[ui->lwSoldierPool->currentRow()] == *fireteam[i])
        {
            // they are, now check if the current rows are in sync
            QTextStream(stdout) << "found " << m_actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << " at row " << fireteamList->currentRow() << endl;
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
        QTextStream(stdout) << "didn't find " << m_actors[ui->lwSoldierPool->currentRow()].getFirstInitialLastName() << endl;
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
    for (int i{0}; i < m_actors.size() && !done; ++i)
    {
        // compare if they are the same actor that is selected in the fireteam
        if (m_actors[i] == *fireteam[fireteamList->currentRow()])
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
    Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};

    // check if actor is on fireteam already
    bool onTeam{false};
    for (const auto &element : fireteam)
    {
        if (selectedActor == *element)
        onTeam = true;
    }

    // actor not on team already
    if (!onTeam)
    fireteam.push_back(&selectedActor);

    // actor on team already so this is a promotion
    else if (onTeam && fireteam.size() >= 2)
    {
        // find actor's place on team
        bool done{false};
        for (int i{0}; i < fireteam.size() && !done; ++i)
        {
            // found actor
            if (*fireteam[i] == selectedActor)
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
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};

    // check if actor is on specified team already
    bool onTeam{false};
    for (const auto &element : fireteam)
    {
        if (selectedActor == *element)
        onTeam = true;
    }

    // actor on specified team so remove actor now
    if (onTeam)
    {
        // find actor's place on team
        for (int i{0}; i < fireteam.size(); ++i)
        {
            // found actor
            if (*fireteam[i] == selectedActor)
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
    for (const auto &element : fireteam)
    {
        if (m_actors[ui->lwSoldierPool->currentRow()] == *element)
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
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    bool onAnyTeam{false};

    // check if actor is on alpha team
    for (const auto &element : m_alpha)
    {
        if (selectedActor == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // check if actor is on bravo team
    for (const auto &element : m_bravo)
    {
        if (selectedActor == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // check if actor is on charlie team
    for (const auto &element : m_charlie)
    {
        if (selectedActor == *element)
        {
            ui->pbUnassign->setEnabled(true);
            onAnyTeam = true;
        }
    }

    // not on any of the teams
    if (!onAnyTeam)
        ui->pbUnassign->setEnabled(false);
}

// assigns the selected kit to the passed in actor
void PlatoonSetup::setActorsKit(const Actor &actor)
{
    int kitIndex{ui->lwKits->currentRow()};
    if (actor.getKitPath() == classRifleman)
    {
        m_assignedKitMap.assignKitToActor(m_riflemanKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getKitPath() == classHeavyWeapons)
    {
        m_assignedKitMap.assignKitToActor(m_heavyWeaponsKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getKitPath() == classSniper)
    {
        m_assignedKitMap.assignKitToActor(m_sniperKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getKitPath() == classDemolitions)
    {
        m_assignedKitMap.assignKitToActor(m_demolitionsKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else
    {
        QString errorMsg{"Error in PlatoonSetup::setActorsKit().  Unrecognized kit path: '"};
        errorMsg += actor.getKitPath();
        errorMsg += "' for actor: ";
        errorMsg += actor.getFirstInitialLastName();
        errorMsg += " filename: ";
        errorMsg += actor.getFileName();
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

}

// selects actor's current kit from the kit list
void PlatoonSetup::selectActorsKit()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    int kitIndex{m_assignedKitMap.getKitIndex(selectedActor.getFileName())};
    ui->lwKits->setCurrentRow(kitIndex);
}

// returns a reference to the appropriate kit vector of the selected actor
std::vector<Kit>& PlatoonSetup::getSelectedActorsKits()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    if (selectedActor.getKitPath() == classRifleman)
    {
        return m_riflemanKits;
    }
    else if (selectedActor.getKitPath() == classHeavyWeapons)
    {
        return m_heavyWeaponsKits;
    }
    else if (selectedActor.getKitPath() == classSniper)
    {
        return m_sniperKits;
    }
    else if (selectedActor.getKitPath() == classDemolitions)
    {
        return m_demolitionsKits;
    }
    else
    {
        QString errorMsg{"Error in getSelectedActorsKitPath().  Unrecognized kit path: '"};
        errorMsg += selectedActor.getKitPath();
        errorMsg += "' for actor: ";
        errorMsg += selectedActor.getFirstInitialLastName();
        errorMsg += " filename: ";
        errorMsg += selectedActor.getFileName();
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

// build kit pool from kits
void PlatoonSetup::buildKitPool(const std::vector<Kit> &kits)
{
    // first delete all current entries in the kit pool
    while (ui->lwKits->count() > 0)
    {
        delete ui->lwKits->takeItem(0);
    }
    QString item1{""};
    QString item2{""};
    for (const auto &element : kits)
    {

        // the first item in a kit is always a gun
        item1 = element.getSlot1FileName();
        for (const auto &element2 : m_guns)
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
            for (const auto &element2 : m_guns)
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
            for (const auto &element2 : m_projectiles)
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
            for (const auto &element2 : m_items)
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
            item2 = m_strings.getString("WPN_EXTRAAMMO");
        }
        // nothing else matched - must be an error
        else
        {
            item2 = "error resolving item2 while building kit list";
            // maybe put popup error and end program here?
        }
        //QString label{item1 + " + " item2};
        item1 += " + ";
        item1 += item2; // add item2 to item1 to create the final label for the list widget
        new QListWidgetItem(item1, ui->lwKits);
    }
}

// updates the box that displays the currently selected kits name
void PlatoonSetup::updateKitNameBox()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    QString kitName{""};
    if (selectedActor.getClassName() == classDemolitions)
    {
        kitName = "DEMO KIT: ";
    }
    else
    {
        kitName = selectedActor.getClassName().toUpper() + " KIT: ";
    }
    QString kitNumber{QString::number(ui->lwKits->currentRow() + 1)};
    ui->leKitName->setText(kitName + kitNumber);
}

// updates all the kit detail boxs (mag cap, range, etc) to the currently selected kit when called
void PlatoonSetup::updateSelectedKitInfo(const std::vector<Kit> &kits)
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
    for (const auto &element : m_guns)
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
        for (const auto &element : m_guns)
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
        for (const auto &element : m_projectiles)
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
        for (const auto &element : m_items)
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
        ui->leName2->setText(m_strings.getString("WPN_EXTRAAMMO") + "   " + totalRoundsText + "/" + currentKit.getExtraAmmo());
    }
}

// updates the big soldier name box as well as soldier stats to the selected soldier
void PlatoonSetup::updateSoldierDetails()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    QString soldierName{selectedActor.getClassName().toUpper() + " " + selectedActor.getName()};
    ui->leSoldierName->setText(soldierName);
    ui->leWeapon->setText("Weapon: " + selectedActor.getWeaponStat());
    ui->leStamina->setText("Endurance: " + selectedActor.getStaminaStat());
    ui->leStealth->setText("Stealth: " + selectedActor.getStealthStat());
    ui->leLeadership->setText("Leadership: " + selectedActor.getLeadershipStat());
}

// enables / disables the apply button
void PlatoonSetup::updateApplyButton()
{
    if (m_alpha.size() <= 0 && m_bravo.size() <= 0 && m_charlie.size() <= 0)
        ui->pbApply->setDisabled(true);
    else
        ui->pbApply->setEnabled(true);
}
