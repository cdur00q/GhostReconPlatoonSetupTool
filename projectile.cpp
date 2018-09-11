#include "projectile.h"
#include "variables.h"
#include "strings.h"

#include <fstream>
#include <QString>
#include <QMessageBox>
#include <QTextStream> // for printing to console

Projectile::Projectile(QString fileName, std::ifstream &projectileFile, const Strings &strings)
    : m_fileName(fileName)
{
    getGameData(m_nameTokenTag, m_nameToken, projectileFile);
    m_name = strings.getString(m_nameToken);
}

// reads and stores one item of game data from a projectile file
fileReadResult Projectile::getGameData(const QString &targetTag, QString &valueToFill, std::ifstream &projectileFile)
{
    // only operate if the file stream is working
    while (projectileFile.good())
    {
        QString value{""};
        QString curString{""};
        char curChar;
        projectileFile.seekg(0);
        while (projectileFile)
        {
            projectileFile.get(curChar);
            // found the first char of an item tag
            if (curChar == '<')
            {
                curString += curChar;
                // read the next chars
                for (int i{0}; i < targetTag.size() - 1; ++i)
                {
                    curString += projectileFile.get();
                    if (curString[curString.size() - 1] == '>')
                        break; // stop reading if a tag closing symbol is encountered
                }
                // then compare if this is the right tag
                if (curString == targetTag)
                {
                    // it is, now extract the value
                    curChar = projectileFile.get();
                    while (curChar != '<')
                    {
                        value += curChar;
                        curChar = projectileFile.get();
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
        // whole file searched and no match
        if (projectileFile.eof())
        {
            projectileFile.clear(); // this function works on a stream reference so clear the eof bit so the stream is left in good standing
        }
        return fileReadResult::NOTFOUND;
    }

    // something went wrong with the file stream
    QMessageBox msgBox(QMessageBox::Critical, "Error", "File stream failure in Projectile::getGameData().");
    msgBox.exec();
    exit(EXIT_FAILURE);

}

Projectile& Projectile::operator= (const Projectile &projectile)
{
    if (this == &projectile)
    {
        return *this;
    }
    m_fileName = projectile.m_fileName;
    m_nameToken = projectile.m_nameToken;
    m_name = projectile.m_name;
    return *this;
}

void Projectile::print() const
{
    QTextStream(stdout) << m_fileName <<
    " " << m_nameToken << " " << m_name << '\n';
}
