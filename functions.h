#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <fstream>
#include <vector>
#include <QString>
#include <QUrl>

#include "actor.h"
#include "strings.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "kit.h"
#include "assignedkitmap.h"
#include "kitrestrictionlist.h"

namespace fs = std::experimental::filesystem;

// Generate a random number between min and max (inclusive)
long getRandomNumber(long min, long max);

// takes a QString and returns the file extension
QString getFileExtension(const QString &fileName);

// takes a file path as a std::string and returns it as a QUrl
QUrl stringToQUrl(const std::string &string);

// reads in all kits from the passed in directory and it's subdirectories and stores them in a kit vector
// maintains a list of discovered kit file names so that only the first discoverd kit will be processed just like the game does
void readInAllKits(const std::string &kitsDirectoryPath, std::vector<Kit> &kitVector);

// adds kits from the passed in source kit vector to the destination kit vector based on whether or not a kit's path matches the passed in kit path
void updateKitVectorPerKitPath(const QString &targetKitPath, const std::vector<Kit> &source, std::vector<Kit> &destination);

// adds/updates kits from passed in kit vector to passed in soldier class specific kit vectors according to the passed in kit restriction list
// kits are checked one at a time and added to each soldier class that is a user of that kit
// a kit could be used by more than one soldier class
void updateKitVectorsPerRestrictionList(const std::vector<Kit> &allKits, const KitRestrictionList &kitList, std::vector<Kit> &riflemanKits, std::vector<Kit> &heavyWeaponsKits, std::vector<Kit> &sniperKits, std::vector<Kit> &demolitionsKits);

// adds/updates kits from passed in kit vector to passed in soldier class specific kit vector according to the passed in kit restriction list
void updateKitVectorPerRestrictionList(const std::vector<Kit> &allKits, const KitRestrictionList &kitList, const QString &soldierClass, std::vector<Kit> &soldierKitVector);

// reads in actor, gun, projectile, or item files
// pass in a directory where actor, gun, projectile, or item files are held, the file extension of the desired file type, and a vector of the desired file type to store the results
template <typename T>
void readInGameFiles(const std::string &directoryPath, const QString &targetFileExtension, T &gameDataVector)
{
    for (const auto &element : fs::directory_iterator(directoryPath))
    {
        std::ifstream currentFile;
        std::error_code errorCode; // no actual error handling will take place with this error code
        if (fs::is_regular_file(element.path(), errorCode))
        {
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
        }
        currentFile.close();
    }
}

// pass in a directory where actor files are held and a vector of actors.  will update the passed in vector with newer versions of the files it finds
void updateActorFiles(const std::string &actorDirectoryPath, std::vector<Actor> &actors);

// updates/adds game data that is relevant to this program(actor data, kit data, etc) from a passed in ghost recon mod directory
void loadMod(const std::string &modPath, std::vector<Actor> &actors, Strings &strings, std::vector<Gun> &guns, std::vector<Projectile> &projectiles, std::vector<Item> &items, std::vector<Kit> &tempKits, KitRestrictionList &kitList, std::string &musicAction3, std::string &musicLoad1, std::string &musicLoad3, std::string &soundButton, std::string &soundApply);

// randomly chooses actors from one vector and places them into another vector if that actor isn't already in there
// loops until it can find an actor from the source that isn't in the destination
void assignRandomActorToVector(const std::vector<Actor> &source, std::vector<Actor> &destination);

// creates an avatar file the game can read from.  by default creates a singleplayer version but can create a cooperative multiplayer version if the forCooperative flag is true
void writeAvatarFile(const std::vector<Actor*> &alpha, const std::vector<Actor*> &bravo, const std::vector<Actor*> &charlie, const AssignedKitMap &assignedKitMap, std::ofstream &file, bool forCooperative);

#endif // FUNCTIONS_H
