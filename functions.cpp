#include "functions.h"
#include "variables.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

/*
// sync the selection boxes
// check if selected soldier is on any of the teams and select them there if so
void syncSelectionBoxes(const std::vector<actor> soldierPool, const int soldierPoolCurRow, const std::vector<actor*> fireteam, Ui::PlatoonSetup *ui)
{
    for (int i{0}; i < fireteam.size(); ++i)
    {
        if (soldierPool[soldierPoolCurRow] == *fireteam[i])
        {
            ui->lwAlpha->setCurrentRow(i);
            i = fireteam.size();
        }
        else
        {
            ui->lwAlpha->clearSelection();
        }
    }
}
*/

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

/*
fileReadResult getGameData(QString target, std::ifstream &file, std::vector<actor> &actors)
{
    // only operate if the file stream is working
    while (file.good())
    {
        //QString target{"<ActorName>"};
        QString value{""};
        QString curString{""};
        char curChar;
        file.seekg(0);
        while (file)
        {
            file.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < target.size() - 1; ++i)
                {
                    curString += file.get();
                }
                QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the right tag
                if (curString == target)
                {
                    // it is, now extract the value
                    curChar = file.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = file.get();
                    }
                    QTextStream(stdout) << "matched " << curString << " value is " << value << '\n';
                    return fileReadResult::FOUND;
                }
                else
                {
                    QTextStream(stdout) << "no match" << endl;
                    curString = "";
                }
            }
        }
        // whole file searched and no match
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in getGameData().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

}
*/
