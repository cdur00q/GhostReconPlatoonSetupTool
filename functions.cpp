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

int writeCoopAvatar(std::ofstream &file)
{
    // only operate if the file stream is working
    while (file.good())
    {

        // working with quotes
        // printf(R"(She said "time flies like an arrow, but fruit flies like a banana".)");
        // https://stackoverflow.com/questions/12338818/how-to-get-double-quotes-into-a-string-literal#12338826
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
        file << data;
        return 0;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in writeCoopAvatar().");
    msgBox.exec();
    return -1;
    //return fileReadResult::FILESTREAMERROR;
}
