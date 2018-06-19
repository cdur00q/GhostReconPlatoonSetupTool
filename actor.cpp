#include "actor.h"
#include "variables.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

actor::actor(std::ifstream &actorFile)
{
    getGameData(m_nameTag, m_name, actorFile);
}

fileReadResult actor::getGameData(QString targetTag, QString &valueToFill, std::ifstream &actorFile)
{
    // only operate if the file stream is working
    while (actorFile.good())
    {
        QString value{""};
        QString curString{""};
        char curChar;
        actorFile.seekg(0);
        while (actorFile)
        {
            actorFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < targetTag.size() - 1; ++i)
                {
                    curString += actorFile.get();
                }
                QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the right tag
                if (curString == targetTag)
                {
                    // it is, now extract the value
                    curChar = actorFile.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = actorFile.get();
                    }
                    QTextStream(stdout) << "matched " << curString << " value is " << value << '\n';
                    valueToFill = value;
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
