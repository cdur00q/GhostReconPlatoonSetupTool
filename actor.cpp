#include "actor.h"
#include "variables.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

Actor::Actor(QString fileName, std::ifstream &actorFile)
    : m_fileName(fileName)
{
    getGameData(m_nameTag, m_name, actorFile);
    getGameData(m_kitPathTag, m_kitPath, actorFile);
    getGameData(m_weaponStatTag, m_weaponStat, actorFile);
    getGameData(m_staminaStatTag, m_staminaStat, actorFile);
    getGameData(m_stealthStatTag, m_stealthStat, actorFile);
    getGameData(m_leadershipStatTag, m_leadershipStat, actorFile);
}

fileReadResult Actor::getGameData(QString targetTag, QString &valueToFill, std::ifstream &actorFile)
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
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                //QTextStream(stdout) << "extracted " << curString << endl;
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
                    //QTextStream(stdout) << "matched " << curString << " value is " << value << '\n';
                    valueToFill = value;
                    return fileReadResult::FOUND;
                }
                else
                {
                    //QTextStream(stdout) << "no match" << endl;
                    curString = "";
                }
            }
        }
        // whole file searched and no match
        if (actorFile.eof())
        {
            actorFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Actor::getGameData().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

}

bool operator== (const Actor &actor1, const Actor &actor2)
{
    return (actor1.m_fileName == actor2.m_fileName);
}

bool operator!= (const Actor &actor1, const Actor &actor2)
{
    return !(actor1 == actor2);
}
