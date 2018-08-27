#ifndef STRINGS_H
#define STRINGS_H

#include "variables.h"
#include <QString>
#include <map>


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

    void readFromFile(std::ifstream &stringsFile);

    QString getString(const QString &key) const;
};

#endif // STRINGS_H
