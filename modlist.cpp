#include "modlist.h"

#include <QMessageBox>

ModList::ModList(std::ifstream &modsSetFile)
{
    readFromFile(modsSetFile);
}

void ModList::readFromFile(std::ifstream &modsSetFile)
{
    bool done{false};
    // only operate if the file stream is working
    while (modsSetFile.good() && !done)
    {
        std::string value{""};
        char curChar;
        //modsSetFile.seekg(0);
        while (modsSetFile)
        {
            modsSetFile.get(curChar);
            // found the open quotation mark of a mod path
            if (curChar == '"')
            {
                value += '\\'; // add this extra slash now because this will be used in a file path
                modsSetFile.get(curChar);
                value += curChar;
                modsSetFile.get(curChar);
                // read until the next backslash
                while (curChar != '\\')
                {
                    value += curChar;
                    modsSetFile.get(curChar);
                }
                value += curChar;
                value += '\\'; // add this extra slash now because this will be used in a file path
                modsSetFile.get(curChar);
                // read until the closing quotation mark is found
                while (curChar != '"')
                {
                    value += curChar;
                    modsSetFile.get(curChar);
                }
                // found it
                if (curChar == '"')
                {
                    m_modList.push_back(value); // add mod path to the vector
                    value = "";
                }
            }
        }
        // whole file searched and no match
        if (modsSetFile.eof())
        {
            modsSetFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        done = true;
    }

    // something went wrong with the file stream
    if (!modsSetFile.good())
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in ModList::readFromFile.");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

void ModList::print() const
{
    for (const auto &curMod : m_modList)
        QTextStream(stdout) << QString::fromStdString(curMod) << '\n';
}
