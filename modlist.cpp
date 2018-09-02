#include "modlist.h"

#include <vector>
#include <fstream>
#include <string>
#include <QMessageBox>
#include <QTextStream> // for printing to console

ModList::ModList()
{

}

ModList::ModList(std::ifstream &modsSetFile)
{
    readFromFile(modsSetFile);
}

// reads every mod path from modsset.txt and stores them in a vector
void ModList::readFromFile(std::ifstream &modsSetFile)
{
    bool done{false};
    // only operate if the file stream is working
    while (modsSetFile.good() && !done)
    {
        std::string value{""};
        char curChar;
        while (modsSetFile)
        {
            modsSetFile.get(curChar);
            // found the open quotation mark of a mod path
            // now read in all data until the closing quotation is found
            if (curChar == '"')
            {
                modsSetFile.get(curChar);
                while (curChar != '"')
                {
                    value += curChar;
                    modsSetFile.get(curChar);
                }
                // found closing quotation mark
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
        QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in ModList::readFromFile().");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

void ModList::print() const
{
    for (const auto &curMod : m_modList)
        QTextStream(stdout) << QString::fromStdString(curMod) << '\n';
}
