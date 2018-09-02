#ifndef KITRESTRICTIONLIST_H
#define KITRESTRICTIONLIST_H

#include <fstream>
#include <set>
#include <QString>


class KitRestrictionList
{
private:
    std::set<QString> m_riflemanList;
    std::set<QString> m_demolitionsList;
    std::set<QString> m_heavyWeaponsList;
    std::set<QString> m_sniperList;
    const QString m_riflemanTag{R"(<Actor Name="rifleman">)"};
    const QString m_demolitionsTag{R"(<Actor Name="demolitions">)"};
    const QString m_heavyWeaponsTag{R"(<Actor Name="heavy-weapons">)"};
    const QString m_sniperTag{R"(<Actor Name="sniper">)"};

    enum class getGameDataResult
    {
        DONE
    };

    // reads in and stores a list of all the kits that belong to the passed in soldier class
    getGameDataResult getGameData(const QString &targetTag, std::set<QString> &kitList, std::ifstream &kitRestrictionFile);

public:
    KitRestrictionList(std::ifstream &kitRestrictionFile);

    ~KitRestrictionList()
    {
    }

    // checks if the passed in kit filename is on the list of the passed in soldier class
    // possible soldier classes are: "rifleman", "heavy-weapons", "sniper", and "demolitions"
    bool checkKitAgainstRestrictionList(const QString &soldierClass, const QString &kitName) const;

    void print() const;
};

#endif // KITRESTRICTIONLIST_H
