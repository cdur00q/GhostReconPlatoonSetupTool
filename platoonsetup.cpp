#include "platoonsetup.h"
#include "ui_platoonsetup.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <string>
#include <filesystem>
#include <fstream>
#include <QTextStream> // for printing to console

#include "variables.h"
#include "functions.h"
namespace fs = std::experimental::filesystem;

PlatoonSetup::PlatoonSetup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlatoonSetup)
{
    ui->setupUi(this);

    QFileSystemModel *model = new QFileSystemModel;
    model->setRootPath("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman");
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"));
    //QTextStream(stdout) << "string to print" << endl;
    int count{0};
    fs::directory_iterator a ("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman");
    //QTextStream(stdout) << a ->path() << endl;
    fs::path b(a->path());
    for (auto& element : fs::directory_iterator("C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman"))
    {
        ++count;


        //QTextStream(stdout) << element << endl;
    }
    std::vector<actor> actors;
    std::string fileName{"C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon\\Mods\\Origmiss\\Actor\\rifleman\\rifleman-01.atr"};
    std::ifstream actorFile(fileName);
    if (actorFile.good())
        QTextStream(stdout) << "good" << '\n';
    else
        QTextStream(stdout) << "bad" << '\n';
    getGameData("<ActorName>", actorFile, actors);
}

PlatoonSetup::~PlatoonSetup()
{
    delete ui;
}
