#include "functions.h"
#include "variables.h"

#include <fstream>
#include <vector>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

fileReadResult getGameData(QString target, std::ifstream &file, std::vector<actor> &actors)
{
    // only operate if the file stream is working
    while (file.good())
    {
        //QString target{"<ActorName>"};
        QString value{""};
        QString curString{""};
        char curChar;
        file.seekg(0);
        while (file)
        {
            file.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < target.size() - 1; ++i)
                {
                    curString += file.get();
                }
                QTextStream(stdout) << "extracted " << curString << endl;
                // then compare if this is the right tag
                if (curString == target)
                {
                    // it is, now extract the value
                    curChar = file.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = file.get();
                    }
                    QTextStream(stdout) << "matched " << curString << " value is " << value << '\n';
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
