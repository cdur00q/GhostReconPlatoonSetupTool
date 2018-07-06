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
    /* 4 possibilites
     * 2 guns
     * 1 gun with extra ammo
     * 1 gun & 1 handheld
     * 1 gun & 1 throwable
     */
    // first item is always a gun so just read that in
    getGameData(m_slot1Tag, m_itemFileNameTag, m_slot1ItemFileName, kitFile);
    getGameData(m_slot1Tag, m_MagCountTag, m_slot1MagCount, kitFile);


    // second item can be a gun, throwable, or handheld
    // found 2nd gun
    if (findGameData(m_slot2TagGun, kitFile) == fileReadResult::FOUND)
    {
        // read in it's data
        getGameData(m_slot2TagGun, m_itemFileNameTag, m_slot2ItemFileName, kitFile);
        getGameData(m_slot2TagGun, m_MagCountTag, m_slot2MagCount, kitFile);
        m_type = kitType::TWOGUNS;
    }
    // found throwable
    else if (findGameData(m_slot2TagThrowable, kitFile) == fileReadResult::FOUND)
    {
        getGameData(m_slot2TagThrowable, m_itemFileNameTag, m_slot2ItemFileName, kitFile);
        getGameData(m_slot2TagThrowable, m_ItemCountTag, m_slot2ItemCount, kitFile);
        m_type = kitType::GUNANDTHROWABLE;
    }
    // found handheld
    else if (findGameData(m_slot2TagHandHeld, kitFile) == fileReadResult::FOUND)
    {
        getGameData(m_slot2TagHandHeld, m_itemFileNameTag, m_slot2ItemFileName, kitFile);
        getGameData(m_slot2TagHandHeld, m_ItemCountTag, m_slot2ItemCount, kitFile);
        m_type = kitType::GUNANDHANDHELD;
    }
    // didn't find any slot 2 tags, must be extra ammo in slot 1
    else
    {
        getGameData(m_slot1Tag, m_extraAmmoTag, m_extraAmmo, kitFile);
        m_type = kitType::GUNANDAMMO;
    }
}

fileReadResult Kit::findGameData(QString targetSlotTag, std::ifstream &kitFile)
{
    // only operate if the file stream is working
    while (kitFile.good())
    {
        QString curString{""};
        char curChar;
        kitFile.seekg(0);
        while (kitFile)
        {
            kitFile.get(curChar);
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
                // then compare if this is the target slot tag
                if (curString == targetSlotTag)
                {
                    return fileReadResult::FOUND;
                }
                else
                {
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
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Kit::findGameData().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

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

void Kit::print() const
{
    QTextStream(stdout) << m_fileName <<
    " " << m_slot1ItemFileName <<
    " x" << m_slot1MagCount;
    if (m_type == kitType::TWOGUNS)
    {
        QTextStream(stdout) << " + " << m_slot2ItemFileName <<
        " x" << m_slot2MagCount << '\n';
    }
    else if (m_type == kitType::GUNANDTHROWABLE || m_type == kitType::GUNANDHANDHELD)
    {
        QTextStream(stdout) << " + " << m_slot2ItemFileName <<
        " x" << m_slot2ItemCount << '\n';
    }
    else if (m_type == kitType::GUNANDAMMO)
    {
        QTextStream(stdout) << " + " << "extra ammo" <<
        " (" << m_extraAmmo << ") " << '\n';
    }

}
