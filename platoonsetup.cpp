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
        for (int i{0}; i < alpha.size(); ++i)
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
    for (int i{0}; i < ui->lwAlpha->count(); ++i)
    {
        delete ui->lwAlpha->takeItem(i);
    }

    // and recreate list based on new backend
    if (alpha.size() > 0)
    {
        for (auto &element : alpha)
        {
            new QListWidgetItem(element->getName(), ui->lwAlpha);
        }
    }

    // debug info
    QTextStream(stdout) << "new backend" << endl;
    printActorVector(alpha);

    // team empty
    if (alpha.size() == 0)
    {
        /*
        alpha.push_back(actors[selectedActor]);
        QListWidgetItem *alpha1 = new QListWidgetItem;
        alpha1->setText(actors[selectedActor].getName());
        ui->lwAlpha->insertItem(0, alpha1);
        */
    }

    /*
    actors[selectedActor].print();
    QTextStream(stdout) << ui->lwSoldierPool->currentRow() << endl;
    ui->lwSoldierPool->clearSelection();
    QTextStream(stdout) << ui->lwSoldierPool->currentRow() << endl;
    */

}

void PlatoonSetup::on_lwSoldierPool_itemSelectionChanged()
{
    ui->lwAlpha->clearSelection();
    //QTextStream(stdout) << ui->lwSoldierPool->currentRow() << endl;

    int selectedActor{ui->lwSoldierPool->currentRow()};
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
    ui->lwSoldierPool->clearSelection();
    QTextStream(stdout) << ui->lwAlpha->currentRow() << endl;
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
                    QTextStream(stdout) << "actor last" << endl;
                    printActorVector(alpha);
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
                    QTextStream(stdout) << "actor not last" << endl;
                    printActorVector(alpha);
                }
            }
        }
    }

    // backend done, now update front end
    // delete all current entries
    for (int i{0}; i < ui->lwAlpha->count(); ++i)
    {
        delete ui->lwAlpha->takeItem(i);
    }

    // and recreate list based on new backend
    if (alpha.size() > 0)
    {
        for (auto &element : alpha)
        {
            new QListWidgetItem(element->getName(), ui->lwAlpha);
        }
    }

    ui->lwAlpha->clearSelection();
    ui->lwSoldierPool->clearSelection();
}
