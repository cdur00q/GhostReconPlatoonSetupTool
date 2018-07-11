#include "gun.h"
#include "variables.h"
#include "strings.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

Gun::Gun(QString fileName, std::ifstream &gunFile, const Strings &strings)
    : m_fileName(fileName)
{
    getGameData(m_nameTokenTag, m_nameToken, gunFile);
    m_name = strings.getString(m_nameToken);
    getGameData(m_magCapTag, m_magCap, gunFile);
    getGameData(m_maxRangeTag, m_maxRange, gunFile);
    getGameData(m_muzzleVelocityTag, m_muzzleVelocity, gunFile);
    getGameData(m_recoilTag, m_recoil, gunFile);
    getGameData(m_maxAccuracyTag, m_maxAccuracy, gunFile);
    getGameData(m_stabilizationTimeTag, m_stabilizationTime, gunFile);
    getGameData(m_silencedTag, m_silenced, gunFile);
    getFireModes(fireModes, gunFile, 0);
    getMaxZoom(m_maxZoom, gunFile, 0);
}

fileReadResult Gun::getGameData(QString targetTag, QString &valueToFill, std::ifstream &gunFile)
{
    // only operate if the file stream is working
    while (gunFile.good())
    {
        QString value{""};
        QString curString{""};
        char curChar;
        gunFile.seekg(0);
        while (gunFile)
        {
            gunFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < targetTag.size() - 1; ++i)
                {
                    curString += gunFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                //QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the right tag
                if (curString == targetTag)
                {
                    // it is, now extract the value
                    curChar = gunFile.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = gunFile.get();
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
        if (gunFile.eof())
        {
            gunFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Gun::getGameData().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

}

fileReadResult Gun::getFireModes(std::vector<FireMode> &fireModes, std::ifstream &gunFile, int startReadingPos)
{
    // only operate if the file stream is working
    while (gunFile.good())
    {
        QString rofEntryTag{"<SelectiveOption"};
        FireMode fireMode;
        QString value{""};
        QString curString{""};
        char curChar;
        gunFile.seekg(startReadingPos);
        while (gunFile)
        {
            gunFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < rofEntryTag.size() - 1; ++i)
                {
                    curString += gunFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                //QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the right tag
                if (curString == rofEntryTag)
                {
                    // it is, now find the open quote of the ROF value
                    curChar = gunFile.get();
                    while (curChar != '"')
                    {
                        curChar = gunFile.get();
                    }
                    // now extract the ROF value
                    curChar = gunFile.get();
                    while (curChar != '"')
                    {
                        value += curChar;
                        curChar = gunFile.get();
                    }
                    fireMode.rpm = value;
                    value = "";
                    // 2nd part, find the fire mode, either "RoundsPerPull" or "IsFullAuto"
                    curChar = gunFile.get();
                    while (curChar != 'R' && curChar != 'I')
                    {
                        curChar = gunFile.get();
                    }
                    // found an 'R' or an 'I'.
                    if (curChar == 'R')
                    {
                        // find the rounds per pull value opening quote
                        while (curChar != '"')
                        {
                            curChar = gunFile.get();
                        }
                        // extract the value
                        curChar = gunFile.get();
                        while (curChar != '"')
                        {
                            value += curChar;
                            curChar = gunFile.get();
                        }
                        fireMode.mode = value;
                        fireModes.push_back(fireMode); // push the fireMode object this function created onto the referenced fireModes vector
                        getFireModes(fireModes, gunFile, gunFile.tellg()); // search for the next fire mode starting at where this function call left off
                    }
                    else if (curChar == 'I')
                    {
                        // find the is full auto value opening quote
                        while (curChar != '"')
                        {
                            curChar = gunFile.get();
                        }
                        // extract the value
                        curChar = gunFile.get();
                        while (curChar != '"')
                        {
                            value += curChar;
                            curChar = gunFile.get();
                        }
                        // check that "IsFullAuto" was set to "1"
                        if (value == '1')
                        {
                            fireMode.mode = "Full Auto";
                        }
                        else
                        {
                            fireMode.mode = "Not Full Auto";
                        }
                        fireModes.push_back(fireMode); // push the fireMode object this function created onto the referenced fireModes vector
                        getFireModes(fireModes, gunFile, gunFile.tellg());  // search for the next fire mode starting at where this function call left off
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
        if (gunFile.eof())
        {
            gunFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Gun::getFireModes().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;

}

fileReadResult Gun::getMaxZoom(QString &valueToFill, std::ifstream &gunFile, int startReadingPos)
{
    // only operate if the file stream is working
    while (gunFile.good())
    {
        QString zoomEntryTag{"<Zoom>"};
        QString value{""};
        QString curString{""};
        char curChar;
        gunFile.seekg(startReadingPos);
        while (gunFile)
        {
            gunFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < zoomEntryTag.size() - 1; ++i)
                {
                    curString += gunFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                // then compare if this is the right tag
                if (curString == zoomEntryTag)
                {
                    // it is, now extract the value
                    curChar = gunFile.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = gunFile.get();
                    }
                    // compare extracted value to value being referenced
                    // only assign the new value if it's bigger
                    if (value > valueToFill)
                    {
                        valueToFill = value;
                    }
                    getMaxZoom(valueToFill, gunFile, gunFile.tellg()); // search for the next zoom value starting at where this function call left off
                }
                else
                {
                    curString = "";
                }
            }
        }
        // whole file searched and no match
        if (gunFile.eof())
        {
            gunFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Gun::getMaxZoom().");
    msgBox.exec();
    return fileReadResult::FILESTREAMERROR;
}

void Gun::print() const
{
    QTextStream(stdout) << m_fileName <<
    " " << m_nameToken << " " << m_name << '\n'
    << "mag cap: " << m_magCap << '\n'
    << "max range: " << m_maxRange << '\n'
    << "muzzle velocity: " << m_muzzleVelocity << '\n'
    << "recoil: " << m_recoil << '\n'
    << "max accuracy: " << m_maxAccuracy << '\n'
    << "stabilization time: " << m_stabilizationTime << '\n'
    << "silenced: " << m_silenced << '\n'
    << "max zoom: " << m_maxZoom << '\n';

    for (auto const &element : fireModes)
        QTextStream(stdout) << "fire mode: " << element.rpm << " RPM " << element.mode << " rounds per pull" << '\n';
}
