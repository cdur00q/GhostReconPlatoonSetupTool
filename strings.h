#ifndef STRINGS_H
#define STRINGS_H

#include "variables.h"
#include <fstream>
#include <map>
#include <QString>


class Strings
{
private:
    std::map<QString, QString> m_stringsMap;

public:
    Strings();

    Strings(std::ifstream &stringsFile);

    ~Strings()
    {
    }

    // reads all strings in from a strings.txt file and stores them in the strings map
    void readFromFile(std::ifstream &stringsFile);

    QString getString(const QString &key) const;
};

#endif // STRINGS_H
