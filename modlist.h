#ifndef MODLIST_H
#define MODLIST_H

#include <vector>
#include <fstream>
#include <string>

class ModList
{
private:
    std::vector<std::string> m_modList;

public:
    ModList();

    ModList(std::ifstream &modsSetFile);

    ~ModList()
    {

    }

    // reads every mod path from modsset.txt and stores them in a vector
    void readFromFile(std::ifstream &modsSetFile);

    const std::vector<std::string>& getModList() const {return m_modList;}

    void print() const;
};

#endif // MODLIST_H
