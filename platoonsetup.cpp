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
namespace fs = std::experimental::filesystem;

static std::vector<actor> actors;
static std::vector<actor*> alpha;
static uiLists lastSelection{uiLists::NONE};

// for debugging
void printActorVector(const std::vector<actor*> &vec)
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
    for (auto &element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"))
    {
        //fs::path temp(element.path());
        //auto temp2{temp.filename()};
        //QString temp3{QString::fromStdWString(temp.filename())};
        currentFile.open(element.path());
        //actor actor(currentFile);
        QString curFileName{QString::fromStdWString(element.path().filename())};
        actors.push_back(actor(curFileName, currentFile));
        currentFile.close();


        //QTextStream(stdout) << element << endl;
        //++count;
    }

    for (auto &element : actors)
    {
        //element.print();
        new QListWidgetItem(element.getName(), ui->lwSoldierPool);
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

    fireteamButtonClicked(alpha, ui->lwAlpha);
    syncSoldierPoolWithFireteam(alpha, ui->lwAlpha);
    /*
    int selectedActor{ui->lwSoldierPool->currentRow()};

    // check if actor is on alpha team already
    bool onAlpha{false};
    for (auto &element : alpha)
    {
        if (actors[selectedActor] == *element)
        onAlpha = true;
    }

    // actor not on team already
    if (!onAlpha)
    alpha.push_back(&actors[selectedActor]);

    // actor on team already so this is a promotion
    else if (onAlpha && alpha.size() >= 2)
    {
        // find actor's place on team
        bool done{false};
        for (int i{0}; i < alpha.size() && !done; ++i)
        {
            // found actor
            if (*alpha[i] == actors[selectedActor])
            {
                // actor is first, so move to back
                if (i == 0)
                {
                    alpha.push_back(alpha[0]);
                    // and then move everyone else up
                    for (int j{i}; j < alpha.size() - 1; ++j)
                    {
                        //actor* temp{alpha[j]};
                        alpha[j] = alpha[j + 1];
                    }
                    alpha.pop_back();
                    done = true;
                    //alpha[alpha.size() - 1] == temp;
                }

                // actor is not
                if (i > 0)
                {
                    // swap with actor before this actor(promotion)
                    actor* temp{alpha[i - 1]};
                    alpha[i - 1] = alpha[i];
                    alpha[i] = temp;
                }
            }
        }
    }

    // backend done, now update front end
    // delete all current entries
    while (ui->lwAlpha->count() > 0)
    {
        delete ui->lwAlpha->takeItem(0);
        QTextStream(stdout) << "frontend item count: " << ui->lwAlpha->count() << endl;
    }

    // and recreate list based on new backend
    if (alpha.size() > 0)
    {
        for (const auto &element : alpha)
        {
            new QListWidgetItem(element->getName(), ui->lwAlpha);
        }
    }

    // debug info
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(alpha);
    QTextStream(stdout) << "frontend item count: " << ui->lwAlpha->count() << endl;
    */
}

void PlatoonSetup::on_lwSoldierPool_itemSelectionChanged()
{
    int selectedActor{ui->lwSoldierPool->currentRow()};

    /*
    // sync the selection boxes
    // check if selected soldier is on any of the teams and select them there if so
    for (int i{0}; i < alpha.size(); ++i)
    {
        if (actors[selectedActor] == *alpha[i])
        {
            ui->lwAlpha->setCurrentRow(i);
            i = alpha.size();
        }
        else
        {
            ui->lwAlpha->clearSelection();
        }
    }
    */

    syncSoldierPoolWithFireteam(alpha, ui->lwAlpha);
    //syncSelectionBoxes(actors, selectedActor, alpha, ui);

    //ui->lwAlpha->clearSelection();
    //QTextStream(stdout) << ui->lwSoldierPool->currentRow() << endl;



    // check if actor is on alpha team already
    bool onAlpha{false};
    for (auto &element : alpha)
    {
        if (actors[selectedActor] == *element)
        onAlpha = true;
    }

    // it is
    if (onAlpha)
    {
        ui->pbAlpha->setEnabled(true);
        ui->pbUnassign->setEnabled(true);
        lastSelection = uiLists::SOLDIERPOOL;
    }

    // it isn't but there is room
    else if (alpha.size() < fireteamMaxMembers && onAlpha == false)
    {
        ui->pbAlpha->setEnabled(true);
        ui->pbUnassign->setEnabled(false);
        lastSelection = uiLists::SOLDIERPOOL;
    }

    // it isn't and no room on alpha
    else if (alpha.size() >= fireteamMaxMembers && onAlpha == false)
    {
        ui->pbAlpha->setEnabled(false);
        ui->pbUnassign->setEnabled(false);
        lastSelection = uiLists::SOLDIERPOOL;
    }
}

void PlatoonSetup::on_lwAlpha_itemSelectionChanged()
{
    //ui->lwSoldierPool->clearSelection();
    //QTextStream(stdout) << ui->lwAlpha->currentRow() << endl;
}

void PlatoonSetup::on_pbUnassign_clicked()
{
    int selectedActor{ui->lwSoldierPool->currentRow()};

    // check if actor is on alpha team already
    bool onAlpha{false};
    for (auto &element : alpha)
    {
        if (actors[selectedActor] == *element)
        onAlpha = true;
    }

    // actor on alpha team so remove actor now
    if (onAlpha)
    {
        // find actor's place on team
        for (int i{0}; i < alpha.size(); ++i)
        {
            // found actor
            if (*alpha[i] == actors[selectedActor])
            {
                // actor is last so just remove
                if (i == alpha.size() - 1)
                {
                    alpha.pop_back();
                    //QTextStream(stdout) << "actor last" << endl;
                    //printActorVector(alpha);
                }
                // actor not last
                else
                {
                    // schuffle everyone up one
                    for (int j{i}; j < alpha.size() - 1; ++j)
                    {
                        //actor* temp{alpha[i]};
                        alpha[j] = alpha[j + 1];
                    }
                    alpha.pop_back(); // and remove last
                    //QTextStream(stdout) << "actor not last" << endl;
                    //printActorVector(alpha);
                }
            }
        }
    }

    // backend done, now update front end
    // delete all current entries
    while (ui->lwAlpha->count() > 0)
    {
        delete ui->lwAlpha->takeItem(0);
    }

    // and recreate list based on new backend
    if (alpha.size() > 0)
    {
        for (const auto &element : alpha)
        {
            new QListWidgetItem(element->getName(), ui->lwAlpha);
        }
    }

    // debug info
    /*
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(alpha);
    QTextStream(stdout) << "frontend item count: " << ui->lwAlpha->count() << endl;
    */

    ui->lwAlpha->clearSelection();
    ui->lwSoldierPool->clearSelection();
}

// sync the selection boxes
// check if selected soldier is on any of the teams and select them there if so
void PlatoonSetup::syncSoldierPoolWithFireteam(const std::vector<actor*> &fireteam, QListWidget *fireteamList)
{
    for (int i{0}; i < fireteam.size(); ++i)
    {
        if (actors[ui->lwSoldierPool->currentRow()] == *fireteam[i])
        {
            //PlatoonSetup::ui->
            fireteamList->setCurrentRow(i);
            i = fireteam.size();
        }
        else
        {
            fireteamList->clearSelection();
        }
    }
}

void PlatoonSetup::fireteamButtonClicked(std::vector<actor *> &fireteam, QListWidget *fireteamList)
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
                    actor* temp{fireteam[i - 1]};
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
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(fireteam);
    QTextStream(stdout) << "frontend item count: " << fireteamList->count() << endl;
}
