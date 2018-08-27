#include "strings.h"
#include "variables.h"

#include <fstream>
#include <QString>
#include <QMessageBox>
#include <utility> // for std::make_pair

Strings::Strings()
{

}

Strings::Strings(std::ifstream &stringsFile)
{
    readFromFile(stringsFile);
}

// reads all strings in from a strings.txt file and stores them in the strings map
void Strings::readFromFile(std::ifstream &stringsFile)
{
    bool done{false};
    // only operate if the file stream is working
    while (stringsFile.good() && !done)
    {
        QString key{""};
        QString value{""};
        char curChar;
        stringsFile.seekg(0);
        while (stringsFile)
        {
            stringsFile.get(curChar);
            // found a quotation mark
            if (curChar == '"')
            {
                stringsFile.get(curChar);
                // read until the next quotation mark
                while (curChar != '"')
                {
                    key += curChar;
                    stringsFile.get(curChar);
                }
                stringsFile.get(curChar);
                // key is done, now find the opening quote of the value
                while (curChar != '"')
                {
                    stringsFile.get(curChar);
                }
                // found it
                if (curChar == '"')
                {
                    stringsFile.get(curChar);
                    // read until the next quotation mark
                    while (curChar != '"')
                    {
                        value += curChar;
                        stringsFile.get(curChar);
                    }
                    stringsFile.get(curChar);
                    m_stringsMap.insert(std::make_pair(key, value)); // insert this round's key and value into the strings map
                    key = ""; // clear key and value for use in the next round
                    value = "";
                }
            }
        }
        // whole file searched and no match
        if (stringsFile.eof())
        {
            stringsFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        done = true;
    }

    // something went wrong with the file stream
    if (!stringsFile.good())
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Strings::readFromFile().");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

// returns a string from the string map or "string not found" if supplied key doesn't exist
QString Strings::getString(const QString &key) const
{
    auto tempIterator{m_stringsMap.find(key)};
    if (tempIterator != m_stringsMap.end())
    {
        return tempIterator->second;
    }
    else
    {
        return "string not found";
    }
}
