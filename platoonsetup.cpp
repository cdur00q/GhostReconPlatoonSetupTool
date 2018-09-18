#include "platoonsetup.h"
#include "ui_platoonsetup.h"

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <map>
#include <utility> // for std::make_pair
#include <QMessageBox>
#include <QtMultimedia/QMediaPlayer>

#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "kit.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "strings.h"
#include "assignedkitmap.h"
#include "kitrestrictionlist.h"
#include "modlist.h"

namespace fs = std::experimental::filesystem;


PlatoonSetup::PlatoonSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatoonSetup)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint); // removes resize arrows when hovering over the border
    PlatoonSetup::grabKeyboard(); // send all keboard input to the main window to prevent messing up the selection logic of the fireteam/soldier pool boxes
    ui->pteFireModes1->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
    ui->pteFireModes2->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);

    // create the actor vectors and reserve space
    std::vector<Actor> rifleman;
    std::vector<Actor> heavyWeapons;
    std::vector<Actor> sniper;
    std::vector<Actor> demolitions;
    rifleman.reserve(76);
    heavyWeapons.reserve(60);
    sniper.reserve(40);
    demolitions.reserve(59);
    m_actors.reserve(36);

    std::ifstream currentFile;
    std::error_code errorCode; // no actual error handling will take place with this error code
    bool errorLoadingBaseGameData{false};
    QString errorMessage{""};

    // read in all actors
    if (fs::is_directory(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\rifleman", errorCode) && !errorLoadingBaseGameData)
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\rifleman", actorExtension, rifleman);
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find directory: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Actor\\rifleman";
    }

    if (fs::is_directory(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\heavy-weapons", errorCode) && !errorLoadingBaseGameData)
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\heavy-weapons", actorExtension, heavyWeapons);
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find directory: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Actor\\heavy-weapons";
    }

    if (fs::is_directory(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\sniper", errorCode) && !errorLoadingBaseGameData)
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\sniper", actorExtension, sniper);
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find directory: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Actor\\sniper";
    }

    if (fs::is_directory(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\demolitions", errorCode) && !errorLoadingBaseGameData)
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Actor\\demolitions", actorExtension, demolitions);
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find directory: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Actor\\demolitions";
    }

    // randomly choose nine actors of each class and put them into the actors pool
    if (rifleman.size() > 0) for (int i{0}; i < 76; ++i) { assignRandomActorToVector(rifleman, m_actors); }
    if (heavyWeapons.size() > 0) for (int i{0}; i < 60; ++i) { assignRandomActorToVector(heavyWeapons, m_actors); }
    if (sniper.size() > 0) for (int i{0}; i < 40; ++i) { assignRandomActorToVector(sniper, m_actors); }
    if (demolitions.size() > 0) for (int i{0}; i < 59; ++i) { assignRandomActorToVector(demolitions, m_actors); }

    // read in strings
    if (fs::is_regular_file(mainGameDirectory + "\\Data\\Shell\\strings.txt", errorCode) && !errorLoadingBaseGameData)
    {
        currentFile.open(mainGameDirectory + "\\Data\\Shell\\strings.txt");
        m_strings.readFromFile(currentFile);
        currentFile.close();
    }
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find file: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Data\\Shell\\strings.txt";
    }

    // read in guns, projectiles, and items
    if (fs::is_directory(mainGameDirectory + "\\Mods\\Origmiss\\Equip", errorCode) && !errorLoadingBaseGameData)
    {
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", gunExtension, m_guns);
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", projectileExtension, m_projectiles);
        readInGameFiles(mainGameDirectory + "\\Mods\\Origmiss\\Equip", itemExtension, m_items);
    }
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find directory: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Equip";
    }

    // read in all discovered kits and store them into a temporary kit vector
    std::vector<Kit> tempKits;
    tempKits.reserve(71); // 71 kits in Mods\Origmiss\Kits
    readInAllKits(mainGameDirectory + "\\Mods\\Origmiss", tempKits);

    // create the kit restriction list
    KitRestrictionList kitList;
    if (fs::is_regular_file(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\quick_missions.qmk", errorCode) && !errorLoadingBaseGameData)
    {
        currentFile.open(mainGameDirectory + "\\Mods\\Origmiss\\Kits\\quick_missions.qmk");
        kitList.readFromFile(currentFile);
        currentFile.close();
    }
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find file: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\Mods\\Origmiss\\Kits\\quick_missions.qmk";
    }

    // create music tracks and sfx
    std::string musicAction3{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\action3.wav"};
    std::string musicLoad1{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\load1.wav"};
    std::string musicLoad3{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\Music\\load3.wav"};
    std::string soundButton{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\I_main1.wav"};
    std::string soundApply{mainGameDirectory + "\\Mods\\Origmiss\\Sound\\I_launch5.wav"};

    // check music tracks and sfx exist
    bool errorLoadingBaseGameMusic{false};
    QString musicErrorMessage{""};
    if (!fs::is_regular_file(musicAction3, errorCode))
    {
        musicErrorMessage = "Warning in PlatoonSetup::PlatoonSetup().  Failed to find music file: ";
        musicErrorMessage += QString::fromStdString(musicAction3);
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }
    if (!fs::is_regular_file(musicLoad1, errorCode))
    {
        musicErrorMessage = "Warning in PlatoonSetup::PlatoonSetup().  Failed to find music file: ";
        musicErrorMessage += QString::fromStdString(musicLoad1);
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }
    if (!fs::is_regular_file(musicLoad3, errorCode))
    {
        musicErrorMessage = "Warning in PlatoonSetup::PlatoonSetup().  Failed to find music file: ";
        musicErrorMessage += QString::fromStdString(musicLoad3);
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }

    if (!fs::is_regular_file(soundButton, errorCode))
    {
        musicErrorMessage = "Warning in PlatoonSetup::PlatoonSetup().  Failed to find sound file: ";
        musicErrorMessage += QString::fromStdString(soundButton);
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }
    if (!fs::is_regular_file(soundApply, errorCode))
    {
        musicErrorMessage = "Warning in PlatoonSetup::PlatoonSetup().  Failed to find sound file: ";
        musicErrorMessage += QString::fromStdString(soundApply);
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }

    // report warnings if music or sfx is missing
    if (errorLoadingBaseGameMusic)
    {
        QMessageBox msgBox(QMessageBox::Warning, "Warning", musicErrorMessage);
        msgBox.exec();
    }

    // read in the mod list
    ModList modList;
    if (fs::is_regular_file(mainGameDirectory + "\\modsset.txt", errorCode) && !errorLoadingBaseGameData)
    {
        currentFile.open(mainGameDirectory + "\\modsset.txt");
        modList.readFromFile(currentFile);
        currentFile.close();
    }
    else if (!errorLoadingBaseGameData)
    {
        errorLoadingBaseGameData = true;
        errorMessage = "Error in PlatoonSetup::PlatoonSetup().  Failed to find file: ";
        errorMessage += QString::fromStdString(mainGameDirectory) + "\\modsset.txt";
    }

    // report errors and end program if any base game data is missing
    if (errorLoadingBaseGameData)
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMessage);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

    // check if any actors were actually loaded as the checks above only guarantee the actor directories exist but don't ensure there are actor files within those directories
    if (m_actors.size() <= 0)
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in PlatoonSetup::PlatoonSetup().  Failed to find any actor files.");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

    // load mods
    for (const auto &currentMod : modList.getModList())
    {
        loadMod(mainGameDirectory + currentMod, m_actors, m_strings, m_guns, m_projectiles, m_items, tempKits, kitList, musicAction3, musicLoad1, musicLoad3, soundButton, soundApply);
    }

    // have the guns, projectiles, and items get their names from the strings object (must do after loading mods as mods may changes strings)
    for (auto &element : m_guns) {element.setNameFromStrings(m_strings);}
    for (auto &element : m_projectiles) {element.setNameFromStrings(m_strings);}
    for (auto &element : m_items) {element.setNameFromStrings(m_strings);}

    // identify the kit paths of each soldier class (must do after loading mods as mods may change actor kit paths)
    QString riflemanKitPath{"no kit path"};
    QString supportKitPath{"no kit path"};
    QString sniperKitPath{"no kit path"};
    QString demolitionsKitPath{"no kit path"};
    for (const auto &element : m_actors) // iterate through all actors and assign/reassign as the different soldier classes are encoutnered
    {
        if (element.getClassName() == classRifleman)
            riflemanKitPath = element.getKitPath();
        else if (element.getClassName() == classSupport || element.getClassName() == classHeavyWeapons)
            supportKitPath = element.getKitPath();
        else if (element.getClassName() == classSniper)
            sniperKitPath = element.getKitPath();
        else if (element.getClassName() == classDemolitions)
            demolitionsKitPath = element.getKitPath();
        else
        {
            QString errorMsg{"Error while assigning kit paths.  Unrecognized soldier class: '"};
            errorMsg += element.getClassName();
            errorMsg += "' for actor: '";
            errorMsg += element.getFirstInitialLastName();
            errorMsg += "' filename: ";
            errorMsg += element.getFileName();
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    }

    // add kits to actor kit vectors
    updateKitVectorPerKitPath(riflemanKitPath, tempKits, m_riflemanKits);
    updateKitVectorPerKitPath(supportKitPath, tempKits, m_heavyWeaponsKits);
    updateKitVectorPerKitPath(sniperKitPath, tempKits, m_sniperKits);
    updateKitVectorPerKitPath(demolitionsKitPath, tempKits, m_demolitionsKits);

    // add kits from the quick_missions.qmk file only if all four soldier classes are using the default kit paths
    if (riflemanKitPath == defaultRiflemanKitPath && supportKitPath == defaultSupportKitPath && sniperKitPath == defaultSniperKitPath && demolitionsKitPath == defaultDemolitionsKitPath)
    {
        updateKitVectorsPerRestrictionList(tempKits, kitList, m_riflemanKits, m_heavyWeaponsKits, m_sniperKits, m_demolitionsKits);
    }

    // check that each soldier class has at least one kit in their kit vector and close program if not (must do after loading mods as mods may add kits)
    if (m_riflemanKits.size() <= 0 || m_heavyWeaponsKits.size() <= 0 || m_sniperKits.size() <= 0 || m_demolitionsKits.size() <= 0)
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in PlatoonSetup::PlatoonSetup().  Failed to find at least one kit for each soldier class.");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

    // build solider pool from actors (must do after loading mods as mods may change actor names)
    for (const auto &element : m_actors)
    {
        if (element.getClassName() == classDemolitions)
        {
            if (element.getFirstInitialLastName().size() >= 13) // for longer names
                new QListWidgetItem(element.getFirstInitialLastName().leftJustified(30) + '\t' + "DEMO", ui->lwSoldierPool); // pad them with single spaces and add a single tab
            else
                new QListWidgetItem(element.getFirstInitialLastName() + "\t\t" + "DEMO", ui->lwSoldierPool); // otherwise just add two tabs
        }
        else if (element.getClassName() == classHeavyWeapons)
        {
            if (element.getFirstInitialLastName().size() >= 13)
                new QListWidgetItem(element.getFirstInitialLastName().leftJustified(30) + '\t' + "SUPPORT", ui->lwSoldierPool);
            else
                new QListWidgetItem(element.getFirstInitialLastName() + "\t\t" + "SUPPORT", ui->lwSoldierPool);
        }
        else
        {
            if (element.getFirstInitialLastName().size() >= 13)
                new QListWidgetItem(element.getFirstInitialLastName().leftJustified(30) + '\t' + element.getClassName().toUpper(), ui->lwSoldierPool);
            else
                new QListWidgetItem(element.getFirstInitialLastName() + "\t\t" + element.getClassName().toUpper(), ui->lwSoldierPool);
        }
    }

    // assign default kits to actors.  the default kits are the first kits of all the availble kits
    int defaultKitIndex{0};
    for (const auto &element : m_actors)
    {
        if (element.getClassName() == classRifleman)
        {
            m_assignedKitMap.assignKitToActor(m_riflemanKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getClassName() == classSupport || element.getClassName() == classHeavyWeapons)
        {
            m_assignedKitMap.assignKitToActor(m_heavyWeaponsKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getClassName() == classSniper)
        {
            m_assignedKitMap.assignKitToActor(m_sniperKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else if (element.getClassName() == classDemolitions)
        {
            m_assignedKitMap.assignKitToActor(m_demolitionsKits[defaultKitIndex].getFileName(), defaultKitIndex, element.getFileName());
        }
        else
        {
            QString errorMsg{"Error while assigning default kits.  Unrecognized soldier class: '"};
            errorMsg += element.getClassName();
            errorMsg += "' for actor: '";
            errorMsg += element.getFirstInitialLastName();
            errorMsg += "' filename: ";
            errorMsg += element.getFileName();
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
    }

    // select first soldier (can only do this after actors and kits are loaded and processed)
    ui->lwSoldierPool->setCurrentRow(0);
    on_lwSoldierPool_itemPressed();

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

    // assign button sfx to button media players
    m_mediaPlayerButtonClick->setVolume(33);
    m_mediaPlayerButtonClick->setMedia(stringToQUrl(soundButton));
    m_mediaPlayerApplyClick->setVolume(33);
    m_mediaPlayerApplyClick->setMedia(stringToQUrl(soundApply));
}

PlatoonSetup::~PlatoonSetup()
{
    delete ui;
    delete m_mediaPlayer;

    // remove read only from avatar files
    std::error_code errorCode; // no error handling will take place with this error code
    fs::permissions(mainGameDirectory + "\\Data\\Temp\\avatar.toe", fs::perms::add_perms | fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, errorCode);
    fs::permissions(mainGameDirectory + "\\Data\\Temp\\coop_avatar.toe", fs::perms::add_perms | fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, errorCode);
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();
    updateApplyButton();
    m_mediaPlayerButtonClick->play();
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();
    updateApplyButton();
    m_mediaPlayerButtonClick->play();
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();
    updateApplyButton();
    m_mediaPlayerButtonClick->play();
}

void PlatoonSetup::on_lwSoldierPool_itemPressed()
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();

    updateSoldierDetails();
}

void PlatoonSetup::on_lwAlpha_itemPressed()
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();

    updateSoldierDetails();
}

void PlatoonSetup::on_lwBravo_itemPressed()
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();

    updateSoldierDetails();
}

void PlatoonSetup::on_lwCharlie_itemPressed()
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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();

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
    updateApplyKitToFireteamButton();
    updateApplyKitToSquadButton();
    updateApplyButton();
    m_mediaPlayerButtonClick->play();
}

void PlatoonSetup::on_lwKits_itemPressed()
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
    m_mediaPlayerButtonClick->play();
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
    m_mediaPlayerButtonClick->play();
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

    if (fireteamPtr) // if fireteam pointer is not null (as in the selected actor is on one of the fireteams)
    {
        // iterate through whatever fireteam the selected actor is on and assign their kit to any other actors of the same soldier class
        for (const auto &element : *fireteamPtr)
        {
            if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                    || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                    || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
                setActorsKit(*element);
        }
    }
    m_mediaPlayerButtonClick->play();
}

void PlatoonSetup::on_pbKitSquad_clicked()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};

    // iterate through all three fireteams and assign the selected actor's kit to any other actors of the same soldier class
    for (const auto &element : m_alpha)
    {
        if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
            setActorsKit(*element);
    }

    for (const auto &element : m_bravo)
    {
        if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
            setActorsKit(*element);
    }

    for (const auto &element : m_charlie)
    {
        if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
            setActorsKit(*element);
    }
    m_mediaPlayerButtonClick->play();
}

void PlatoonSetup::on_pbApply_clicked()
{
    // write single player avatar file
    std::error_code errorCode; // no error handling will take place with this error code
    std::string avatarPath{mainGameDirectory + "\\Data\\Temp\\avatar.toe"};
    fs::permissions(avatarPath, fs::perms::add_perms | fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, errorCode);
    std::ofstream avatar(avatarPath, std::ios::out|std::ios::trunc);
    writeAvatarFile(m_alpha, m_bravo, m_charlie, m_assignedKitMap, avatar, false);
    avatar.close();
    fs::permissions(avatarPath, fs::perms::remove_perms & fs::perms::owner_write & fs::perms::group_write & fs::perms::others_write, errorCode);

    // write cooperative avatar file
    std::string coopAvatarPath{mainGameDirectory + "\\Data\\Temp\\coop_avatar.toe"};
    fs::permissions(coopAvatarPath, fs::perms::add_perms | fs::perms::owner_write | fs::perms::group_write | fs::perms::others_write, errorCode);
    std::ofstream coopAvatar(coopAvatarPath, std::ios::out|std::ios::trunc);
    writeAvatarFile(m_alpha, m_bravo, m_charlie, m_assignedKitMap, coopAvatar, true);
    coopAvatar.close();
    fs::permissions(coopAvatarPath, fs::perms::remove_perms & fs::perms::owner_write & fs::perms::group_write & fs::perms::others_write, errorCode);

    m_mediaPlayerApplyClick->play();
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
// check if selected soldier is on the specified team and select them there if so
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
            fireteamList->setCurrentRow(i);
            done = true;
            noMatch = false;
        }
    }
    if (noMatch)
    {
        fireteamList->clearSelection();
    }
}

// sync a selection from the specified fireteam with the soldier pool
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
    if (noMatch) // there should always be a match
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
    }

    // and recreate list based on new backend
    if (fireteam.size() > 0)
    {
        for (const auto &element : fireteam)
        {
            new QListWidgetItem(element->getFirstInitialLastName(), fireteamList);
        }
    }
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
    if (actor.getClassName() == classRifleman)
    {
        m_assignedKitMap.assignKitToActor(m_riflemanKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getClassName() == classSupport || actor.getClassName() == classHeavyWeapons)
    {
        m_assignedKitMap.assignKitToActor(m_heavyWeaponsKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getClassName() == classSniper)
    {
        m_assignedKitMap.assignKitToActor(m_sniperKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else if (actor.getClassName() == classDemolitions)
    {
        m_assignedKitMap.assignKitToActor(m_demolitionsKits[kitIndex].getFileName(), kitIndex, actor.getFileName());
    }
    else
    {
        QString errorMsg{"Error in PlatoonSetup::setActorsKit().  Unrecognized soldier class: '"};
        errorMsg += actor.getClassName();
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
    if (selectedActor.getClassName() == classRifleman)
    {
        return m_riflemanKits;
    }
    else if (selectedActor.getClassName() == classSupport || selectedActor.getClassName() == classHeavyWeapons)
    {
        return m_heavyWeaponsKits;
    }
    else if (selectedActor.getClassName() == classSniper)
    {
        return m_sniperKits;
    }
    else if (selectedActor.getClassName() == classDemolitions)
    {
        return m_demolitionsKits;
    }
    else
    {
        QString errorMsg{"Error in PlatoonSetup::getSelectedActorsKits().  Unrecognized soldier class: '"};
        errorMsg += selectedActor.getClassName();
        errorMsg += "' for actor: ";
        errorMsg += selectedActor.getFirstInitialLastName();
        errorMsg += " filename: ";
        errorMsg += selectedActor.getFileName();
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

// builds kit pool from passed in kit vector
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
            QString errorMsg{"Error in PlatoonSetup::buildKitPool().  Failed to resolve item two in kit: "};
            errorMsg += element.getFileName();
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
            exit(EXIT_FAILURE);
        }
        item1 += " + ";
        item1 += item2; // add item2 to item1 to create the final label for the list widget
        new QListWidgetItem(item1, ui->lwKits);
    }
}

// updates the box that displays the currently selected kit's name
void PlatoonSetup::updateKitNameBox()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    QString kitName{""};
    if (selectedActor.getClassName() == classDemolitions)
    {
        kitName = "DEMO KIT: ";
    }
    else if (selectedActor.getClassName() == classHeavyWeapons)
    {
        kitName = "SUPPORT KIT: ";
    }
    else
    {
        kitName = selectedActor.getClassName().toUpper() + " KIT: ";
    }
    QString kitNumber{QString::number(ui->lwKits->currentRow() + 1)};
    ui->leKitName->setText(kitName + kitNumber);
}

// updates all the kit detail boxes (mag cap, range, etc) to the currently selected kit
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
        QString errorMsg{"Error in PlatoonSetup::updateSelectedKitInfo().  Unable to find first item of selected kit '"};
        (errorMsg += currentKit.getFileName()) += "'";
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
        msgBox.exec();
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
        weaponTypeText = "AR / LMG / SMG";
    else if (weaponType == "2")
        weaponTypeText = "Sniper Rifle";
    else if (weaponType == "3")
        weaponTypeText = "Bolt Action Rifle";
    else if (weaponType == "4")
        weaponTypeText = "Grenade Launcher";
    else if (weaponType == "5")
        weaponTypeText = "Rocket Launcher";
    else if (weaponType == "6")
        weaponTypeText = "Shotgun";
    ui->leWeaponType1->setText(weaponTypeText);
    if (gun->getSilenced().toInt() >= 1) // 0 means no, 1 means yes, but in centcom mod(and others?) 2 is used for yes, which GR engine accepts
    {
        ui->leSilenced1->setText("Yes");
    }
    else
    {
        ui->leSilenced1->setText("No");
    }
    ui->leMaxZoom1->setText(gun->getMaxZoom() + "x");
    QString fireModesForBox{""};
    bool firstFireMode{true};
    for (auto &element : gun->getFireModes())
    {
        if (!firstFireMode)
            fireModesForBox += '\n';
        else
            firstFireMode = false;
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
            QString errorMsg{"Error in PlatoonSetup::updateSelectedKitInfo().  Unable to find second item of selected kit '"};
            (errorMsg += currentKit.getFileName()) += "'";
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
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
            weaponTypeText = "AR / LMG / SMG";
        else if (weaponType == "2")
            weaponTypeText = "Sniper Rifle";
        else if (weaponType == "3")
            weaponTypeText = "Bolt Action Rifle";
        else if (weaponType == "4")
            weaponTypeText = "Grenade Launcher";
        else if (weaponType == "5")
            weaponTypeText = "Rocket Launcher";
        else if (weaponType == "6")
            weaponTypeText = "Shotgun";
        ui->leWeaponType2->setText(weaponTypeText);
        if (gun2->getSilenced().toInt() >= 1)
        {
            ui->leSilenced2->setText("Yes");
        }
        else
        {
            ui->leSilenced2->setText("No");
        }
        ui->leMaxZoom2->setText(gun2->getMaxZoom() + "x");
        fireModesForBox = "";
        bool firstFireMode{true};
        for (auto &element : gun2->getFireModes())
        {
            if (!firstFireMode)
                fireModesForBox += '\n';
            else
                firstFireMode = false;
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
            QString errorMsg{"Error in PlatoonSetup::updateSelectedKitInfo().  Unable to find second item of selected kit '"};
            (errorMsg += currentKit.getFileName()) += "'";
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
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
            QString errorMsg{"Error in PlatoonSetup::updateSelectedKitInfo().  Unable to find second item of selected kit '"};
            (errorMsg += currentKit.getFileName()) += "'";
            QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
            msgBox.exec();
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
    QString soldierName{""};
    if (selectedActor.getClassName() == classHeavyWeapons)
        soldierName = "SUPPORT " + selectedActor.getName();
    else
        soldierName = selectedActor.getClassName().toUpper() + " " + selectedActor.getName();
    ui->leSoldierName->setText(soldierName);
    ui->leWeapon->setText("Weapon: " + selectedActor.getWeaponStat());
    ui->leStamina->setText("Endurance: " + selectedActor.getStaminaStat());
    ui->leStealth->setText("Stealth: " + selectedActor.getStealthStat());
    ui->leLeadership->setText("Leadership: " + selectedActor.getLeadershipStat());
}

// enables / disables the apply kit to fireteam button
void PlatoonSetup::updateApplyKitToFireteamButton()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};
    std::vector<Actor*> *fireteamPtr{nullptr};

    // check if selected actor is on fireteam alpha
    for (const auto &element : m_alpha)
    {
        if (selectedActor == *element) // they are
            fireteamPtr = &m_alpha; // set temporary pointer to alpha
    }
    for (const auto &element : m_bravo) { if (selectedActor == *element) fireteamPtr = &m_bravo; }
    for (const auto &element : m_charlie) { if (selectedActor == *element) fireteamPtr = &m_charlie; }

    if (fireteamPtr) // if fireteam pointer is not null (as in the selected actor is on one of the fireteams)
    {
        // iterate through whatever fireteam the selected actor is on and count the number of team members of the same soldier class as the selected actor
        int soldiersOfSameClass{0};
        for (const auto &element : *fireteamPtr)
        {
            if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                    || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                    || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
                ++soldiersOfSameClass;
        }
        if (soldiersOfSameClass >= 1) // counted 1 or more actors on the same fireteam of the same soldier class
            ui->pbKitFireteam->setEnabled(true);
        else
            ui->pbKitFireteam->setDisabled(true);
    }
    else
    {
        ui->pbKitFireteam->setDisabled(true);
    }
}

// enables / disables the apply kit to squad button
void PlatoonSetup::updateApplyKitToSquadButton()
{
    const Actor &selectedActor{m_actors[ui->lwSoldierPool->currentRow()]};

    // first check if selected actor is on the squad (on any fireteam)
    bool selectedActorOnSquad{false};
    for (const auto &element : m_alpha) { if (selectedActor == *element) selectedActorOnSquad = true; }
    for (const auto &element : m_bravo) { if (selectedActor == *element) selectedActorOnSquad = true; }
    for (const auto &element : m_charlie) { if (selectedActor == *element) selectedActorOnSquad = true; }

    if (selectedActorOnSquad)
    {
        // iterate through all three fireteams and compare the soldier class of the selected actor against all the other actors
        // and tally up how many matches there are
        int soldiersOfSameClass{0};
        for (const auto &element : m_alpha)
        {
            if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                    || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                    || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
                ++soldiersOfSameClass;
        }

        for (const auto &element : m_bravo)
        {
            if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                    || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                    || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
                ++soldiersOfSameClass;
        }

        for (const auto &element : m_charlie)
        {
            if ((element->getClassName() == selectedActor.getClassName() && *element != selectedActor)
                    || (element->getClassName() == classSupport && selectedActor.getClassName() == classHeavyWeapons)
                    || (element->getClassName() == classHeavyWeapons && selectedActor.getClassName() == classSupport))
                ++soldiersOfSameClass;
        }

        if (soldiersOfSameClass >= 1) // counted 1 or more actors accross all three fireteams of the same class
            ui->pbKitSquad->setEnabled(true);
        else
            ui->pbKitSquad->setDisabled(true);
    }
    else
        ui->pbKitSquad->setDisabled(true);
}

// enables / disables the apply button
void PlatoonSetup::updateApplyButton()
{
    if (m_alpha.size() <= 0 && m_bravo.size() <= 0 && m_charlie.size() <= 0)
        ui->pbApply->setDisabled(true);
    else
        ui->pbApply->setEnabled(true);
}
