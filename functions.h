#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"
#include "actor.h"
#include "strings.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "kit.h"
#include "assignedkitmap.h"
#include "kitrestrictionlist.h"
#include <vector>
#include <fstream>
#include <QString>
#include <QMessageBox>

namespace fs = std::experimental::filesystem;

long getRandomNumber(long min, long max);

QString getFileExtension(const QString &fileName);

// read in all kits from the passed in directory and it's subdirectories and store them in a kit vector
void readInAllKits(const std::string &kitsDirectoryPath, std::vector<Kit> &kitVector);

// adds/updates kits from passed in kit list to passed in soldier class specific kit lists according to the passed in kit restriction list
// kits are checked one at a time and added to each soldier class that is a user of that kit
// a kit could be used by more than one soldier class
void updateKitVectorPerRestrictionList(const std::vector<Kit> &allKits, const KitRestrictionList &kitList, std::vector<Kit> &riflemanKits, std::vector<Kit> &heavyWeaponsKits, std::vector<Kit> &sniperKits, std::vector<Kit> &demolitionsKits);

// read in actor or kit files
// pass in a directory where actor or kit files are held, the file extension of the desired file type, and a vector of the desired file type to store the results
template <typename T>
void readInGameFiles(const std::string &directoryPath, const QString &targetFileExtension, T &gameDataVector)
{
    for (const auto &element : fs::directory_iterator(directoryPath))
    {
        std::ifstream currentFile;
        QString curFileName{QString::fromStdWString(element.path().filename())};
        if (QString::compare(getFileExtension(curFileName), targetFileExtension, Qt::CaseInsensitive) == 0)  // check the extension of the current iteration file matches what was passed in
        {
            currentFile.open(element.path());
            if (!currentFile.good())
            {
                QString errorMsg{"Error in readInGameFiles().  Failed to open file: "};
                errorMsg += QString::fromStdWString(element.path());
                QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
                msgBox.exec();
                exit(EXIT_FAILURE);
            }
            bool replacedItem{false};
            for (auto &element2 : gameDataVector)
            {
                if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0) // found an item in the vector with same filename as this one, replace it with this new one
                {
                    element2 = typename T::value_type(curFileName, currentFile);
                    replacedItem = true;
                }
            }
            if (!replacedItem) // didn't find an item in the vector with this name already so add in this new item
            {
                gameDataVector.push_back(typename T::value_type(curFileName, currentFile));
            }
        }
        currentFile.close();
    }
}

// read gun, projectile, or item files
// pass in a directory where gun, projectile, or item files are held, the file extension of the desired file type, and a vector of the desired file type to store the results
template <typename T>
void readInGameFiles(const std::string &directoryPath, const QString &targetFileExtension, T &gameDataVector, const Strings &strings)
{
    for (const auto &element : fs::directory_iterator(directoryPath))
    {
        std::ifstream currentFile;
        QString curFileName{QString::fromStdWString(element.path().filename())};
        if (QString::compare(getFileExtension(curFileName), targetFileExtension, Qt::CaseInsensitive) == 0)  // check the extension of the current iteration file matches what was passed in
        {
            currentFile.open(element.path());
            if (!currentFile.good())
            {
                QString errorMsg{"Error in readInGameFiles().  Failed to open file: "};
                errorMsg += QString::fromStdWString(element.path());
                QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
                msgBox.exec();
                exit(EXIT_FAILURE);
            }
            bool replacedItem{false};
            for (auto &element2 : gameDataVector)
            {
                if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0) // found an item in the vector with same filename as this one, replace it with this new one
                {
                    element2 = typename T::value_type(curFileName, currentFile, strings);
                    replacedItem = true;
                }
            }
            if (!replacedItem) // didn't find an item in the vector with this name already so add in this new item
            {
                gameDataVector.push_back(typename T::value_type(curFileName, currentFile, strings));
            }
        }
        currentFile.close();
    }
}

// pass in a directory where actor files are held and a vector of actors.  will update the passed in vector with newer versions of the files it finds
void updateActorFiles(const std::string &actorDirectoryPath, std::vector<Actor> &actors);

void loadMod(const std::string &modPath, std::vector<Actor> &actors, Strings &strings, std::vector<Gun> &guns, std::vector<Projectile> &projectiles, std::vector<Item> &items, std::vector<Kit> &riflemanKits, std::vector<Kit> &heavyWeaponsKits, std::vector<Kit> &sniperKits, std::vector<Kit> &demolitionsKits);

void assignRandomActorToVector(const std::vector<Actor> &source, std::vector<Actor> &destination);

void writeCoopAvatar(const std::vector<Actor*> &alpha, const std::vector<Actor*> &bravo, const std::vector<Actor*> &charlie, const AssignedKitMap &assignedKitMap, std::ofstream &file);

#endif // FUNCTIONS_H
