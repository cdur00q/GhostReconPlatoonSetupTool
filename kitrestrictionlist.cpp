#include "kitrestrictionlist.h"

#include <fstream>
#include <set>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

KitRestrictionList::KitRestrictionList(std::ifstream &kitRestrictionFile)
{
    getGameData(m_riflemanTag, m_riflemanList, kitRestrictionFile);
    getGameData(m_demolitionsTag, m_demolitionsList, kitRestrictionFile);
    getGameData(m_heavyWeaponsTag, m_heavyWeaponsList, kitRestrictionFile);
    getGameData(m_sniperTag, m_sniperList, kitRestrictionFile);
}

// reads in and stores a list of all the kits that belong to the passed in soldier class
KitRestrictionList::getGameDataResult KitRestrictionList::getGameData(const QString &targetTag, std::set<QString> &kitList, std::ifstream &kitRestrictionFile)
{
    // only operate if the file stream is working
    while (kitRestrictionFile.good())
    {
        QString value{""};
        QString curString{""};
        char curChar;
        kitRestrictionFile.seekg(0);
        while (kitRestrictionFile)
        {
            kitRestrictionFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < targetTag.size() - 1; ++i)
                {
                    curString += kitRestrictionFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                // then compare if this is the right tag
                if (curString == targetTag)
                {
                    curString = "";
                    // now read in all the kit file names in the list
                    // stop reading when "</" is encountered, the first characters of </Actor>
                    while (kitRestrictionFile)
                    {
                        if (curChar == '<' && kitRestrictionFile.peek() == '/')
                        {
                            return getGameDataResult::DONE;
                        }
                        if (curChar == '"') // found opening quote of filename
                        {
                            kitRestrictionFile.get(curChar);
                            while (curChar != '"') // read the rest of the filename
                            {
                                curString += curChar;
                                kitRestrictionFile.get(curChar);
                            }
                            kitList.insert(curString); // insert the new name into the kit list
                            curString = "";
                        }
                        kitRestrictionFile.get(curChar);
                    }
                }
                else
                {
                    curString = "";
                }
            }
        }
        // whole file searched and no match
        if (kitRestrictionFile.eof())
        {
            kitRestrictionFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return getGameDataResult::DONE;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in KitRestrictionList::getGameData().");
    msgBox.exec();
    exit(EXIT_FAILURE);
}

// checks if the passed in kit filename is on the list of the passed in soldier class
// possible soldier classes are: "rifleman", "heavy-weapons", "sniper", and "demolitions"
bool KitRestrictionList::checkKitAgainstRestrictionList(const QString &soldierClass, const QString &kitName)
{
    const std::set<QString>* listToCheck{nullptr};
    if (soldierClass == "rifleman")
    {
        listToCheck = &m_riflemanList;
    }
    else if (soldierClass == "demolitions")
    {
        listToCheck = &m_demolitionsList;
    }
    else if (soldierClass == "heavy-weapons")
    {
        listToCheck = &m_heavyWeaponsList;
    }
    else if (soldierClass == "sniper")
    {
        listToCheck = &m_sniperList;
    }
    else
    {
        QString errorMsg{"Error, unrecognized soldier class: '"};
        errorMsg += soldierClass;
        errorMsg += "' passed to KitRestrictionList::checkKitAgainstRestrictionList().";
        QMessageBox msgBox(QMessageBox::Critical, "Error", errorMsg);
        msgBox.exec();
        return false;
    }
    std::set<QString>::iterator it{listToCheck->begin()};
    it = listToCheck->find(kitName);
    if (it != listToCheck->end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void KitRestrictionList::print()
{
    QTextStream(stdout) << "Rifleman Kits: " << '\n';
    std::set<QString>::iterator it{m_riflemanList.begin()};
    while (it != m_riflemanList.end())
    {
        QTextStream(stdout) << *it << '\n';
        ++it;
    }
    QTextStream(stdout) << "Heavy-Weapons Kits: " << '\n';
    it = m_heavyWeaponsList.begin();
    while (it != m_heavyWeaponsList.end())
    {
        QTextStream(stdout) << *it << '\n';
        ++it;
    }
    QTextStream(stdout) << "Sniper Kits: " << '\n';
    it = m_sniperList.begin();
    while (it != m_sniperList.end())
    {
        QTextStream(stdout) << *it << '\n';
        ++it;
    }
    QTextStream(stdout) << "Demolitions Kits: " << '\n';
    it = m_demolitionsList.begin();
    while (it != m_demolitionsList.end())
    {
        QTextStream(stdout) << *it << '\n';
        ++it;
    }
}
