#include "kit.h"
#include "variables.h"

#include <fstream>
#include <cmath> // for std::abs()
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

Kit::Kit(QString filePath, QString fileName, std::ifstream &kitFile)
    : m_fileName(fileName)
{
    m_filePaths.insert(filePath);

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

// returns fileReadResult::FOUND if the passed in slot tag exists in the passed in kit file
fileReadResult Kit::findGameData(const QString &targetSlotTag, std::ifstream &kitFile)
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
    exit(EXIT_FAILURE);
}

// reads and stores one item of game data from a kit file
fileReadResult Kit::getGameData(const QString &targetSlotTag, const QString &targetTag, QString &valueToFill, std::ifstream &kitFile)
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
            if (curChar == '<') // found the first char of an item tag
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
                    // it is, now search for the target tag
                    curString = "";
                    while (kitFile)
                    {
                        kitFile.get(curChar);
                        if (curChar == '<') // found the first char of an item tag
                        {
                            curString += curChar;
                            // read the next chars
                            for (int i{0}; i < targetTag.size() - 1; ++i)
                            {
                                curString += kitFile.get();
                                if (curString[curString.size() - 1] == '>')
                                    break; // stop reading if a tag closing symbol is encountered
                            }
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
                                valueToFill = value;
                                return fileReadResult::FOUND;
                            }
                            else
                            {
                                curString = "";
                            }
                        }
                    }
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
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Kit::getGameData().");
    msgBox.exec();
    exit(EXIT_FAILURE);
}

QString Kit::getSlot2FileName() const
{
    if (m_type == kitType::TWOGUNS || m_type == kitType::GUNANDTHROWABLE || m_type == kitType::GUNANDHANDHELD)
    {
        return m_slot2ItemFileName;
    }
    else if (m_type == kitType::GUNANDAMMO)
    {
        return "extra ammo";
    }
    else
    {
        return "error in Kit::getSlot2FileName(): no matching kitType";
    }
}

// checks if the kit contains the passed in file path within it's internal file path list
bool Kit::containsFilePath(const QString &filePath) const
{
    std::set<QString>::const_iterator it{m_filePaths.cbegin()};
    while (it != m_filePaths.cend())
    {
        QString pathForComparision{""};
        int sizeDifference{std::abs(filePath.size() - it->size())};
        for (int i{sizeDifference} ; i < it->size() ; ++i)
        {
            pathForComparision += (*it)[i];
        }
        if (QString::compare(pathForComparision, filePath, Qt::CaseInsensitive) == 0)
            return true;
        ++it;
    }
    return false;
}

Kit& Kit::operator= (const Kit &kit)
{
    if (this == &kit)
    {
        return *this;
    }

    // when assigning one kit to another, combine their file path lists
    std::set<QString>::iterator it{kit.m_filePaths.begin()};
    while (it != kit.m_filePaths.end())
    {
        m_filePaths.insert(*it);
        ++it;
    }

    m_fileName = kit.m_fileName;
    m_slot1ItemFileName = kit.m_slot1ItemFileName;
    m_slot2ItemFileName = kit.m_slot2ItemFileName;
    m_slot1MagCount = kit.m_slot1MagCount;
    m_slot2MagCount = kit.m_slot2MagCount;
    m_slot2ItemCount = kit.m_slot2ItemCount;
    m_extraAmmo = kit.m_extraAmmo;
    m_type = kit.m_type;
    return *this;
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
