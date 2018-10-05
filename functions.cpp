#include "functions.h"

#include <fstream>
#include <vector>
#include <set>
#include <string>
#include <filesystem>
#include <cstdlib> // for rand() and srand()
#include <QString>
#include <QMessageBox>
#include <QUrl>

#include "variables.h"
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
// Assumes srand() has already been called
// Assumes max - min <= RAND_MAX
// from http://www.learncpp.com/cpp-tutorial/59-random-number-generation/
long getRandomNumber(long min, long max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);  // static used for efficiency, so we only calculate this value once
    // evenly distribute the random number across our range
    return min + static_cast<long>((max - min + 1) * (rand() * fraction));
}

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
    // no extension found (turn of popup message box for release)
    /*
    QString errorMessage{"Error in getFileExtension().  No extension could be found for filename: "};
    errorMessage += fileName;
    QMessageBox msgBox(QMessageBox::Critical, "Error", errorMessage);
    msgBox.exec();
    */
    return "error";
}

// takes a file path as a std::string and returns it as a QUrl
QUrl stringToQUrl(const std::string &string)
{
    std::string tempStringOut{""};
    for (const auto &element : string)
    {
        if (element == '\\')
        {
            tempStringOut += '/';
        }
        else
        {
            tempStringOut += element;
        }
    }
    QUrl qUrl{QUrl::fromLocalFile(QString::fromStdString(tempStringOut))};
    return qUrl;
}

// reads in all kits from the passed in directory and it's subdirectories and stores them in a kit vector
// maintains a list of discovered kit file names so that only the first discoverd kit will be processed just like the game does
void readInAllKits(const std::string &kitsDirectoryPath, std::vector<Kit> &kitVector)
{
    std::set<QString> discoveredKits;
    for (const auto &element : fs::recursive_directory_iterator(kitsDirectoryPath))
    {
        std::ifstream currentFile;
        // first check this is a regular file before proceeding
        // using the error code version to avoid exceptions being thrown
        // but no actual error handling will take place with this error code
        std::error_code errorCode;
        if (fs::is_regular_file(element.path(), errorCode))
        {
            fs::path pathNoFilename{element.path()};
            pathNoFilename.remove_filename();
            QString curFilePath{QString::fromStdWString(pathNoFilename)};
            QString curFileName{QString::fromStdWString(element.path().filename())};
            if (QString::compare(getFileExtension(curFileName), kitExtension, Qt::CaseInsensitive) == 0)  // check this is a kit file
            {
                currentFile.open(element.path());
                if (!currentFile.good())
                {
                    QString errorMsg{"Error in readInAllKits().  Failed to open file: "};
                    errorMsg += QString::fromStdWString(element.path());
                    QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
                    msgBox.exec();
                    exit(EXIT_FAILURE);
                }
                // now check if this kit file name has already been seen
                std::set<QString>::const_iterator it{discoveredKits.cbegin()};
                it = discoveredKits.find(curFileName);
                if (it == discoveredKits.cend()) // kit file name hasn't been seen yet, proceed to check it against the passed in kit vector
                {
                    discoveredKits.insert(curFileName); // add this kit file name to the list of discovered kits
                    bool replacedItem{false};
                    for (auto &element2 : kitVector)
                    {
                        if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0) // found a kit in the vector with same filename as this one, replace it with this new one
                        {
                            element2 = Kit(curFilePath, curFileName, currentFile);
                            replacedItem = true;
                        }
                    }
                    if (!replacedItem) // didn't find a kit in the vector with this name already so add in this new kit
                    {
                        kitVector.push_back(Kit(curFilePath, curFileName, currentFile));
                    }
                }
                else // kit file name has been seen before so only add this new kit path to the kit
                {
                    for (auto &element2 : kitVector)
                    {
                        if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0)
                        {
                            element2.addFilePath(curFilePath);
                        }
                    }
                }
            }
        }
        currentFile.close();
    }
}

// adds kits from the passed in source kit vector to the destination kit vector based on whether or not a kit's path matches the passed in kit path
void updateKitVectorPerKitPath(const QString &targetKitPath, const std::vector<Kit> &source, std::vector<Kit> &destination)
{
    for (const auto &element : source)
    {
        if (element.containsFilePath(targetKitPath))
        {
            bool replacedItem{false};
            for (auto &element2 : destination)
            {
                if (QString::compare(element.getFileName(), element2.getFileName(), Qt::CaseInsensitive) == 0) // found a kit in the vector with same filename as this one, replace it with this new one
                {
                    element2 = element;
                    replacedItem = true;
                }
            }
            if (!replacedItem) // didn't find a kit in the vector with this name already so add in this new kit
            {
                destination.push_back(element);
            }
        }
    }
}

// adds/updates kits from passed in kit vector to passed in soldier class specific kit vectors according to the passed in kit restriction list
// kits are checked one at a time and added to each soldier class that is a user of that kit
// a kit could be used by more than one soldier class
void updateKitVectorsPerRestrictionList(const std::vector<Kit> &allKits, const KitRestrictionList &kitList, std::vector<Kit> &riflemanKits, std::vector<Kit> &heavyWeaponsKits, std::vector<Kit> &sniperKits, std::vector<Kit> &demolitionsKits)
{
    std::vector<QString> soldierClasses{classRifleman, classHeavyWeapons, classSniper, classDemolitions}; // the order of these two vectors have to match so if the first element is rifleman here
    std::vector<std::vector<Kit>*> allClassesKits{&riflemanKits, &heavyWeaponsKits, &sniperKits, &demolitionsKits}; // then the first element has to be riflemen here
    // this goes three deep
    // first loop is every single kit
    // second loop is every soldier class
    // third loop is every kit of loop two's soldier class
    for (const auto &potentialKit : allKits) // for every kit
    {
        for (int currentClass{0} ; currentClass < soldierClasses.size(); ++currentClass) // iterating through one of the four soldier classes at a time
        {
            if (kitList.checkKitAgainstRestrictionList(soldierClasses[currentClass], potentialKit.getFileName()) == true) // current kit belongs to current soldier class
            {
                bool replacedKit{false};
                for (auto &existingKit : *allClassesKits[currentClass]) // check if current kit happens to already be in the permanent kit list for current soldier class
                {
                    if (QString::compare(potentialKit.getFileName(), existingKit.getFileName(), Qt::CaseInsensitive) == 0) // it is, so update it with this new one
                    {
                        existingKit = potentialKit;
                        replacedKit = true;
                    }
                }
                if (!replacedKit) // it isn't, so add in this new one
                {
                    allClassesKits[currentClass]->push_back(potentialKit);
                }
            }
        }
    }
}

// adds/updates kits from passed in kit vector to passed in soldier class specific kit vector according to the passed in kit restriction list
void updateKitVectorPerRestrictionList(const std::vector<Kit> &allKits, const KitRestrictionList &kitList, const QString &soldierClass, std::vector<Kit> &soldierKitVector)
{
    for (const auto &potentialKit : allKits) // for every kit
    {
        if (kitList.checkKitAgainstRestrictionList(soldierClass, potentialKit.getFileName()) == true) // current kit belongs to passed in soldier class
        {
            bool replacedKit{false};
            for (auto &existingKit : soldierKitVector) // check if current kit happens to already be in the passed in kit vector
            {
                if (QString::compare(potentialKit.getFileName(), existingKit.getFileName(), Qt::CaseInsensitive) == 0) // it is, so update it with this new one
                {
                    existingKit = potentialKit;
                    replacedKit = true;
                }
            }
            if (!replacedKit) // it isn't, so add in this new one
            {
                soldierKitVector.push_back(potentialKit);
            }
        }
    }
}

// pass in a directory where actor files are held and a vector of actors.  will update the passed in vector with newer versions of the files it finds
void updateActorFiles(const std::string &actorDirectoryPath, std::vector<Actor> &actors)
{
    for (const auto &element : fs::directory_iterator(actorDirectoryPath))
    {
        std::ifstream currentFile;
        QString curFileName{QString::fromStdWString(element.path().filename())};
        if (QString::compare(getFileExtension(curFileName), actorExtension, Qt::CaseInsensitive) == 0)  // check the extension of the current iteration file is an actor file extension
        {
            currentFile.open(element.path());
            if (!currentFile.good())
            {
                QString errorMsg{"Error in updateActorFiles().  Failed to open file: "};
                errorMsg += QString::fromStdWString(element.path());
                QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
                msgBox.exec();
                exit(EXIT_FAILURE);
            }
            for (auto &element2 : actors)
            {
                if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0) // found an actor in the vector with same filename as this one, replace it with this new one
                {
                    element2 = Actor(curFileName, currentFile);
                }
            }
        }
        currentFile.close();
    }
}

// updates/adds game data that is relevant to this program(actor data, kit data, etc) from a passed in ghost recon mod directory
void loadMod(const std::string &modPath, std::vector<Actor> &actors, Strings &strings, std::vector<Gun> &guns, std::vector<Projectile> &projectiles, std::vector<Item> &items, std::vector<Kit> &tempKits, KitRestrictionList &kitList, std::string &musicAction3, std::string &musicLoad1, std::string &musicLoad3, std::string &soundButton, std::string &soundApply)
{
    std::ifstream currentFile;
    std::error_code errorCode; // no actual error handling will take place with this error code

    // update any discovered actors
    if (fs::is_directory(modPath + "\\Actor\\rifleman", errorCode)) // if there is a actor\rifleman directory in this mod
        updateActorFiles(modPath + "\\Actor\\rifleman", actors); // read from it and update the actors vector
    if (fs::is_directory(modPath + "\\Actor\\heavy-weapons", errorCode))
        updateActorFiles(modPath + "\\Actor\\heavy-weapons", actors);
    if (fs::is_directory(modPath + "\\Actor\\sniper", errorCode))
        updateActorFiles(modPath + "\\Actor\\sniper", actors);
    if (fs::is_directory(modPath + "\\Actor\\demolitions", errorCode))
        updateActorFiles(modPath + "\\Actor\\demolitions", actors);

    // update and add strings if mod contains a strings file
    if (fs::is_regular_file(modPath + "\\Shell\\strings.txt", errorCode))
    {
        currentFile.open(modPath + "\\Shell\\strings.txt");
        strings.readFromFile(currentFile);
        currentFile.close();
    }

    // read in guns, projectiles, and items if there is an "equip" folder
    if (fs::is_directory(modPath + "\\Equip", errorCode))
    {
        readInGameFiles(modPath + "\\Equip", gunExtension, guns);
        readInGameFiles(modPath + "\\Equip", projectileExtension, projectiles);
        readInGameFiles(modPath + "\\Equip", itemExtension, items);
    }

    // add/update any new kits discovered in this mod
    if (fs::is_directory(modPath + "\\Kits", errorCode))
        readInAllKits(modPath + "\\Kits", tempKits);

    // if this mod has a kit restriction list file read it
    if (fs::is_regular_file(modPath + "\\Kits\\quick_missions.qmk", errorCode))
    {
        currentFile.open(modPath + "\\Kits\\quick_missions.qmk");
        kitList.readFromFile(currentFile);
        currentFile.close();
    }

    // update the music tracks and sound effects if there are newer versions of them
    for (const auto &element : fs::recursive_directory_iterator(modPath))
    {
        QString curFilePath{QString::fromStdWString(element.path())};
        QString curFileName{QString::fromStdWString(element.path().filename())};
        std::error_code errorCode;
        if ((QString::compare(curFileName, "action3.wav", Qt::CaseInsensitive) == 0) && fs::is_regular_file(element.path(), errorCode))
        {
            musicAction3 = curFilePath.toStdString();
        }
        else if ((QString::compare(curFileName, "load1.wav", Qt::CaseInsensitive) == 0) && fs::is_regular_file(element.path(), errorCode))
        {
            musicLoad1 = curFilePath.toStdString();
        }
        else if ((QString::compare(curFileName, "load3.wav", Qt::CaseInsensitive) == 0) && fs::is_regular_file(element.path(), errorCode))
        {
            musicLoad3 = curFilePath.toStdString();
        }
        else if ((QString::compare(curFileName, "I_main1.wav", Qt::CaseInsensitive) == 0) && fs::is_regular_file(element.path(), errorCode))
        {
            soundButton = curFilePath.toStdString();
        }
        else if ((QString::compare(curFileName, "I_launch5.wav", Qt::CaseInsensitive) == 0) && fs::is_regular_file(element.path(), errorCode))
        {
            soundApply = curFilePath.toStdString();
        }
    }
}

// randomly chooses actors from one vector and places them into another vector if that actor isn't already in there
// loops until it can find an actor from the source that isn't in the destination
void assignRandomActorToVector(const std::vector<Actor> &source, std::vector<Actor> &destination)
{
    bool actorAlreadyPresent{false};
    bool done{false};
    const Actor *actorPtr{nullptr};
    while (!done)
    {
        actorAlreadyPresent = false;
        actorPtr = &source[getRandomNumber(0, source.size() - 1)];
        for (auto &element : destination)
        {
            if (element == *actorPtr)
            {
                actorAlreadyPresent = true;
            }
        }
        if (!actorAlreadyPresent)
        {
            destination.push_back(*actorPtr);
            done = true;
        }
    }
}

// creates an avatar file the game can read from.  by default creates a singleplayer version but can create a cooperative multiplayer version if the forCooperative flag is true
void writeAvatarFile(const std::vector<Actor*> &alpha, const std::vector<Actor*> &bravo, const std::vector<Actor*> &charlie, const AssignedKitMap &assignedKitMap, std::ofstream &avatarFile, bool forCooperative)
{
    // exit if there are no soldiers in any of the passed in fireteams
    if (alpha.size() <= 0 && bravo.size() <= 0 && charlie.size() <= 0)
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error, three empty fireteams passed to writeAvatarFile().");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }

    bool done{false};
    // only operate if the file stream is working
    while (avatarFile.good() && !done)
    {
        QString toeFile{""};
        QString company{""};
        QString platoon{""};
        QString alphaTeam{""};
        QString alphaStrings[fireteamMaxMembers]; // 1 string per team member
        QString bravoTeam{""};
        QString bravoStrings[fireteamMaxMembers];
        QString charlieTeam{""};
        QString charlieStrings[fireteamMaxMembers];
        bool firstSoldier{true}; // the first soldier in the file will have the "owner" value added to it
        int idNumber{0};

        // create alpha team members
        for (int i{0}; i < alpha.size(); ++i)
        {
            alphaStrings[i] = "\n\t\t\t\t";
            alphaStrings[i] += R"(<Actor IgorId = ")";
            alphaStrings[i] += QString::number(++idNumber);
            alphaStrings[i] += R"(" ScriptId = "0" File = ")";
            if (forCooperative) // switch the single player actors the multiplayer actors
            {
                if (alpha[i]->getClassName() == classRifleman) {alphaStrings[i] += "mp_plt1_asl.atr";}
                else if (alpha[i]->getClassName() == classSupport || alpha[i]->getClassName() == classHeavyWeapons) {alphaStrings[i] += "mp_plt1_hvywep.atr";}
                else if (alpha[i]->getClassName() == classSniper) {alphaStrings[i] += "mp_plt1_snip.atr";}
                else if (alpha[i]->getClassName() == classDemolitions) {alphaStrings[i] += "mp_plt1_dem.atr";}
                else
                {
                    QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in writeAvatarFile(), unrecognized soldier class encountered in Alpha Team.");
                    msgBox.exec();
                    exit(EXIT_FAILURE);
                }
            }
            else
                alphaStrings[i] += alpha[i]->getFileName();
            alphaStrings[i] += R"(" Kit = ")";
            alphaStrings[i] += assignedKitMap.getKitFileName(alpha[i]->getFileName());
            if (firstSoldier)
            {
                alphaStrings[i] += R"(" Owner = "0"/>)";
                firstSoldier = false;
            }
            else alphaStrings[i] += R"("/>)";
        }

        // create bravo team members
        for (int i{0}; i < bravo.size(); ++i)
        {
            bravoStrings[i] = "\n\t\t\t\t";
            bravoStrings[i] += R"(<Actor IgorId = ")";
            bravoStrings[i] += QString::number(++idNumber);
            bravoStrings[i] += R"(" ScriptId = "0" File = ")";
            if (forCooperative)
            {
                if (bravo[i]->getClassName() == classRifleman) {bravoStrings[i] += "mp_plt1_asl.atr";}
                else if (bravo[i]->getClassName() == classSupport || bravo[i]->getClassName() == classHeavyWeapons) {bravoStrings[i] += "mp_plt1_hvywep.atr";}
                else if (bravo[i]->getClassName() == classSniper) {bravoStrings[i] += "mp_plt1_snip.atr";}
                else if (bravo[i]->getClassName() == classDemolitions) {bravoStrings[i] += "mp_plt1_dem.atr";}
                else
                {
                    QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in writeAvatarFile(), unrecognized soldier class encountered in Bravo Team.");
                    msgBox.exec();
                    exit(EXIT_FAILURE);
                }
            }
            else
                bravoStrings[i] += bravo[i]->getFileName();
            bravoStrings[i] += R"(" Kit = ")";
            bravoStrings[i] += assignedKitMap.getKitFileName(bravo[i]->getFileName());
            if (firstSoldier)
            {
                bravoStrings[i] += R"(" Owner = "0"/>)";
                firstSoldier = false;
            }
            else bravoStrings[i] += R"("/>)";
        }

        // create charlie team members
        for (int i{0}; i < charlie.size(); ++i)
        {
            charlieStrings[i] = "\n\t\t\t\t";
            charlieStrings[i] += R"(<Actor IgorId = ")";
            charlieStrings[i] += QString::number(++idNumber);
            charlieStrings[i] += R"(" ScriptId = "0" File = ")";
            if (forCooperative)
            {
                if (charlie[i]->getClassName() == classRifleman) {charlieStrings[i] += "mp_plt1_asl.atr";}
                else if (charlie[i]->getClassName() == classSupport || charlie[i]->getClassName() == classHeavyWeapons) {charlieStrings[i] += "mp_plt1_hvywep.atr";}
                else if (charlie[i]->getClassName() == classSniper) {charlieStrings[i] += "mp_plt1_snip.atr";}
                else if (charlie[i]->getClassName() == classDemolitions) {charlieStrings[i] += "mp_plt1_dem.atr";}
                else
                {
                    QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in writeAvatarFile(), unrecognized soldier class encountered in Charlie Team.");
                    msgBox.exec();
                    exit(EXIT_FAILURE);
                }
            }
            else
                charlieStrings[i] += charlie[i]->getFileName();
            charlieStrings[i] += R"(" Kit = ")";
            charlieStrings[i] += assignedKitMap.getKitFileName(charlie[i]->getFileName());
            if (firstSoldier)
            {
                charlieStrings[i] += R"(" Owner = "0"/>)";
                firstSoldier = false;
            }
            else charlieStrings[i] += R"("/>)";
        }

        // create alpha team
        if (alpha.size() > 0)
        {
            alphaTeam = "\n\t\t\t";
            alphaTeam += "<Team IgorId = ";
            idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
            (alphaTeam += '"') += QString::number(idNumber);
            alphaTeam += R"(" ScriptId = "0" Name = "_Alpha">)";
            for (int i{0}; i < alpha.size(); ++i)
            {
                alphaTeam += alphaStrings[i];
            }
            alphaTeam += "\n\t\t\t</Team>";
        }

        // create bravo team
        if (bravo.size() > 0)
        {
            bravoTeam = "\n\t\t\t";
            bravoTeam += "<Team IgorId = ";
            idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
            if (alpha.size() > 0) { ++idNumber;}
            (bravoTeam += '"') += QString::number(idNumber);
            bravoTeam += R"(" ScriptId = "0" Name = "_Bravo">)";
            for (int i{0}; i < bravo.size(); ++i)
            {
                bravoTeam += bravoStrings[i];
            }
            bravoTeam += "\n\t\t\t</Team>";
        }

        // create charlie team
        if (charlie.size() > 0)
        {
            charlieTeam = "\n\t\t\t";
            charlieTeam += "<Team IgorId = ";
            idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
            if (alpha.size() > 0) { ++idNumber;}
            if (bravo.size() > 0) { ++idNumber;}
            (charlieTeam += '"') += QString::number(idNumber);
            charlieTeam += R"(" ScriptId = "0" Name = "_Charlie">)";
            for (int i{0}; i < charlie.size(); ++i)
            {
                charlieTeam += charlieStrings[i];
            }
            charlieTeam += "\n\t\t\t</Team>";
        }

        // create platoon
        platoon = "\n\t\t";
        platoon += "<Platoon IgorId = ";
        idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
        if (alpha.size() > 0) { ++idNumber;}
        if (bravo.size() > 0) { ++idNumber;}
        if (charlie.size() > 0) { ++idNumber;}
        (platoon += '"') += QString::number(idNumber) += '"';
        if (forCooperative)
            platoon += R"( ScriptId = "0" Name = "_Platoon1" MultiplayerPlatoon = "1">)";
        else
            platoon += R"( ScriptId = "0" Name = "_Platoon">)";
        platoon += alphaTeam += bravoTeam += charlieTeam += "\n\t\t</Platoon>";

        // create company
        company = "\n\t";
        company += "<Company IgorId = ";
        ++idNumber; // assumes idNumber hasn't been touched since platoon
        (company += '"') += QString::number(idNumber) += '"';
        if (forCooperative)
            company += R"( ScriptId = "0" Name = "_---" MultiplayerCompany = "2">)";
        else
            company += R"( ScriptId = "0" Name = "_Company">)";
        company += platoon += "\n\t</Company>";

        // create toe file
        toeFile += "<TOEFile>\n\t<VersionNumber>2.000000</VersionNumber>";
        toeFile += company += "\n</TOEFile>";

        // write prepared string to file
        avatarFile << toeFile.toStdString();
        done = true;
    }

    // something went wrong with the file stream
    if (!avatarFile.good())
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in writeAvatarFile().");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}
