#include "kit.h"
#include "variables.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

Kit::Kit(QString fileName, std::ifstream &kitFile)
    : m_fileName(fileName)
{
    /* possibilites
     * 2 guns
     * 1 gun with extra ammo
     * 1 gun & 1 handheld
     * 1 gun & 1 throwable
     */
    getGameData(m_slot1Tag, m_itemFileNameTag, m_slot1ItemFileName, kitFile);
    getGameData(m_slot1Tag, m_MagCountTag, m_slot1MagCount, kitFile);
    fileReadResult lastResult;
    //lastResult = getGameData(m_slot2Tag, m_itemFileNameTag, m_slot2ItemFileName, kitFile);
    getGameData(m_slot1Tag, m_extraAmmoTag, m_extraAmmo, kitFile);
    getGameData(m_slot2Tag, m_MagCountTag, m_slot2MagCount, kitFile);
    // if no item can be found in slot 2 it implies slot 1 has extra ammo
    /*
    if (lastResult == fileReadResult::NOTFOUND)
    {
        // read in the extra ammo
        getGameData(m_slot1Tag, m_extraAmmoTag, m_extraAmmo, kitFile);
        m_itemCount = 1;
    }
    else
    {
        // otherwise slot 2 has an item and read in it's mag count
        getGameData(m_slot2Tag, m_MagCountTag, m_slot2MagCount, kitFile);
        m_itemCount = 2;
    }
    */
}

fileReadResult Kit::getGameData(QString targetSlotTag, QString targetTag, QString &valueToFill, std::ifstream &kitFile)
{
    // only operate if the file stream is working
    while (kitFile.good())
    {
        QString value{""};
        QString curString{""};
        char curChar;
        kitFile.seekg(0);
        while (kitFile)
        {
            kitFile.get(curChar);
            // first find the target slot tag
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < targetSlotTag.size() - 1; ++i)
                {
                    curString += kitFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                //QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the target slot tag
                if (curString == targetSlotTag)
                {
                    // it is, now search for the target tag
                    curString = "";
                    while (kitFile)
                    {
                        kitFile.get(curChar);
                        // found the first char of an item tag
                        if (curChar == '<')
                        {
                            curString += curChar;
                            // read the next chars
                            for (int i{0}; i < targetTag.size() - 1; ++i)
                            {
                                curString += kitFile.get();
                                if (curString[curString.size() - 1] == '>')
                                    break; // stop reading if a tag closing symbol is encountered
                            }
                            //QTextStream(stdout) << "extracted " << curString << endl;
                            // then compare if this is the right tag
                            if (curString == targetTag)
                            {
                                // it is, now extract the value
                                curChar = kitFile.get();
                                while (curChar != '<')
                                {
                                    value += curChar;
                                    curChar = kitFile.get();
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
                }
                else
                {
                    //QTextStream(stdout) << "no match" << endl;
                    curString = "";
                }
            }
        }
        // whole file searched and no match
        if (kitFile.eof())
        {
            kitFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Kit::getGameData().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

}

void Kit::print()
{
    QTextStream(stdout) << m_fileName <<
    " " << m_slot1ItemFileName <<
    " x" << m_slot1MagCount;
    if (m_itemCount == 2)
    {
        QTextStream(stdout) << " + " << m_slot2ItemFileName <<
        " x" << m_slot2MagCount << '\n';
    }
    else if (m_itemCount == 1)
    {
        QTextStream(stdout) << " + " << "extra ammo" <<
        " x" << m_extraAmmo << '\n';
    }

}
