#ifndef MODLIST_H
#define MODLIST_H

#include <vector>
#include <fstream>
#include <string>
#include <QString>
#include <QTextStream> // for printing to console

class ModList
{
private:
    std::vector<std::string> m_modList;

    void readFromFile(std::ifstream &modsSetFile);
public:
    ModList(std::ifstream &modsSetFile);

    ~ModList()
    {

    }

    const std::vector<std::string>& getModList() const {return m_modList;}

    void print() const;
};

#endif // MODLIST_H
