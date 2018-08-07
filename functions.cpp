#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib> // for rand() and srand()
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

#include "variables.h"
#include "functions.h"
#include "actor.h"
#include "strings.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include "kit.h"
#include "assignedkitmap.h"

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
    // no extension found
    QString errorMessage{"Error in getFileExtension().  No extension could be found for filename: "};
    errorMessage += fileName;
    QMessageBox msgBox(QMessageBox::Critical, "Error", errorMessage);
    msgBox.exec();
    return "error";
}

void readInAllKits(const std::string &kitsDirectoryPath, std::vector<Kit> &kitVector)
{
    for (const auto &element : fs::recursive_directory_iterator(kitsDirectoryPath))
    {
        std::ifstream currentFile;
        // first check this is a regular file before proceeding
        // using the error code version to avoid exceptions being thrown
        // but no actual error handling will take place with this error code
        std::error_code errorCode;
        if (fs::is_regular_file(element.path(), errorCode))
        {
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
                bool replacedItem{false};
                for (auto &element2 : kitVector)
                {
                    if (QString::compare(curFileName, element2.getFileName(), Qt::CaseInsensitive) == 0) // found a kit in the vector with same filename as this one, replace it with this new one
                    {
                        element2 = Kit(curFileName, currentFile);
                        replacedItem = true;
                    }
                }
                if (!replacedItem) // didn't find a kit in the vector with this name already so add in this new kit
                {
                    kitVector.push_back(Kit(curFileName, currentFile));
                }
            }
        }
        currentFile.close();
    }
}

// randomly chooses actors from one vector and places them into another
// vector if that actor isn't already in there.
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

int writeCoopAvatar(const std::vector<Actor> &actors, const std::vector<Actor*> &alpha, const std::vector<Actor*> &bravo, const std::vector<Actor*> &charlie, const AssignedKitMap &assignedKitMap, std::ofstream &avatarFile)
{
    // only operate if the file stream is working
    while (avatarFile.good())
    {
        QString toeFile{""};
        QString company{""};
        QString platoon{""};
        QString alphaTeam{""};
        QString alpha1{""};
        QString alpha2{""};
        QString alpha3{""};
        QString alphaStrings[3]; // 1 string per team member
        QString bravoTeam{""};
        QString bravo1{""};
        QString bravo2{""};
        QString bravo3{""};
        QString bravoStrings[3];
        QString charlieTeam{""};
        QString charlie1{""};
        QString charlie2{""};
        QString charlie3{""};
        QString charlieStrings[3];
        bool firstSoldier{true}; // the first soldier in the file will have the "owner" value added to it
        // working with quotes
        // printf(R"(She said "time flies like an arrow, but fruit flies like a banana".)");
        // https://stackoverflow.com/questions/12338818/how-to-get-double-quotes-into-a-string-literal#12338826
        /*
        std::string data{R"(<TOEFile>
                <VersionNumber>2.000000</VersionNumber>
                <Company IgorId = "4" ScriptId = "0" Name = "_---" MultiplayerCompany = "2">
                    <Platoon IgorId = "3" ScriptId = "0" Name = "_Platoon1" MultiplayerPlatoon = "1">
                        <Team IgorId = "2" ScriptId = "0" Name = "_Alpha">
                            <Actor IgorId = "1" ScriptId = "0" Name = "_Actor" File = "mp_plt1_asl.atr" Kit = "rifleman-01.kit" Owner = "0"/>
                        </Team>
                    </Platoon>
                </Company>
            </TOEFile>")"};
            */
        int idNumber{0};
        /*
        QString data{"<TOEFile>"
                     "<VersionNumber>2.000000"
                     "</VersionNumber> <Company IgorId = "};
        (data += '"') += QString::number(idNumber) += '"';
        */
        //data += QString::number(IdNumber);
        //data += '"';

        // create alpha team members
        for (int i{0}; i < alpha.size(); ++i)
        {
            alphaStrings[i] = "\n\t\t\t\t";
            alphaStrings[i] += R"(<Actor IgorId = ")";
            alphaStrings[i] += QString::number(++idNumber);
            alphaStrings[i] += R"(" ScriptId = "0" File = ")";
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

        /*
        // create alpha team member #1
        if (alpha.size() >= 1)
        {
            alpha1 = "\n\t\t\t\t";
            alpha1 += R"(<Actor IgorId = ")";
            alpha1 += QString::number(++idNumber);
            alpha1 += R"(" ScriptId = "0" File = ")";
            alpha1 += alpha[0]->getFileName();
            alpha1 += R"(" Kit = ")";
            alpha1 += assignedKitMap.getKitFileName(alpha[0]->getFileName());
            if (firstSoldier)
            {
                alpha1 += R"(" Owner = "0"/>)";
                firstSoldier = false;
            }
            else alpha1 += R"("/>)";
        }

        // create alpha team member #2
        if (alpha.size() >= 2)
        {
            alpha2 = "\n\t\t\t\t";
            alpha2 += R"(<Actor IgorId = ")";
            alpha2 += QString::number(++idNumber);
            alpha2 += R"(" ScriptId = "0" File = ")";
            alpha2 += alpha[1]->getFileName();
            alpha2 += R"(" Kit = ")";
            alpha2 += assignedKitMap.getKitFileName(alpha[1]->getFileName());
            alpha2 += R"("/>)";
        }

        // create alpha team member #3
        if (alpha.size() == 3)
        {
            alpha3 = "\n\t\t\t\t";
            alpha3 += R"(<Actor IgorId = ")";
            alpha3 += QString::number(++idNumber);
            alpha3 += R"(" ScriptId = "0" File = ")";
            alpha3 += alpha[2]->getFileName();
            alpha3 += R"(" Kit = ")";
            alpha3 += assignedKitMap.getKitFileName(alpha[2]->getFileName());
            alpha3 += R"("/>)";
        }


        // create bravo team member #1
        if (bravo.size() >= 1)
        {
            bravo1 = "\n\t\t\t\t";
            bravo1 += R"(<Actor IgorId = ")";
            bravo1 += QString::number(++idNumber);
            bravo1 += R"(" ScriptId = "0" File = ")";
            bravo1 += bravo[0]->getFileName();
            bravo1 += R"(" Kit = ")";
            bravo1 += assignedKitMap.getKitFileName(bravo[0]->getFileName());
            if (firstSoldier)
            {
                bravo1 += R"(" Owner = "0"/>)";
                firstSoldier = false;
            }
            else bravo1 += R"("/>)";
        }

        // create bravo team member #2
        if (bravo.size() >= 2)
        {
            bravo2 = "\n\t\t\t\t";
            bravo2 += R"(<Actor IgorId = ")";
            bravo2 += QString::number(++idNumber);
            bravo2 += R"(" ScriptId = "0" File = ")";
            bravo2 += bravo[1]->getFileName();
            bravo2 += R"(" Kit = ")";
            bravo2 += assignedKitMap.getKitFileName(bravo[1]->getFileName());
            bravo2 += R"("/>)";
        }

        // create bravo team member #3
        if (bravo.size() == 3)
        {
            bravo3 = "\n\t\t\t\t";
            bravo3 += R"(<Actor IgorId = ")";
            bravo3 += QString::number(++idNumber);
            bravo3 += R"(" ScriptId = "0" File = ")";
            bravo3 += bravo[2]->getFileName();
            bravo3 += R"(" Kit = ")";
            bravo3 += assignedKitMap.getKitFileName(bravo[2]->getFileName());
            bravo3 += R"("/>)";
        }

        */

        // create alpha team
        if (alpha.size() > 0)
        {
            alphaTeam = "\n\t\t\t";
            alphaTeam += "<Team IgorId = ";
            idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
            (alphaTeam += '"') += QString::number(idNumber);
            alphaTeam += R"(" ScriptId = "0" Name = "_Alpha">)";

            //alphaTeam += alpha1 += alpha2 += alpha3 += "\n\t\t\t</Team>";
            alphaTeam += alphaStrings[0] += alphaStrings[1] += alphaStrings[2] += "\n\t\t\t</Team>";
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

            bravoTeam += bravoStrings[0] += bravoStrings[1] += bravoStrings[2] += "\n\t\t\t</Team>";
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

            charlieTeam += charlieStrings[0] += charlieStrings[1] += charlieStrings[2] += "\n\t\t\t</Team>";
        }

        // create platoon
        platoon = "\n\t\t";
        platoon += "<Platoon IgorId = ";
        idNumber = alpha.size() + bravo.size() + charlie.size() + 1;
        if (alpha.size() > 0) { ++idNumber;}
        if (bravo.size() > 0) { ++idNumber;}
        if (charlie.size() > 0) { ++idNumber;}
        (platoon += '"') += QString::number(idNumber) += '"';
        platoon += R"( ScriptId = "0" Name = "_Platoon1" MultiplayerPlatoon = "1">)";

        platoon += alphaTeam += bravoTeam += charlieTeam += "\n\t\t</Platoon>";

        // create company
        company = "\n\t";
        company += "<Company IgorId = ";
        ++idNumber; // assumes idNumber hasn't been touched since platoon
        (company += '"') += QString::number(idNumber) += '"';
        company += R"( ScriptId = "0" Name = "_---" MultiplayerCompany = "2">)";

        company += platoon += "\n\t</Company>";

        // create toe file
        toeFile += "<TOEFile>\n\t<VersionNumber>2.000000</VersionNumber>";
        toeFile += company += "\n</TOEFile>";


        avatarFile << toeFile.toStdString();
        return 0;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in writeCoopAvatar().");
    msgBox.exec();
    return -1;
    //return fileReadResult::FILESTREAMERROR;
}
