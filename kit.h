#ifndef KIT_H
#define KIT_H

#include "variables.h"
#include <QString>
#include <QTextStream> // for printing to console

class Kit
{
public:
    enum class kitType
    {
        TWOGUNS,
        GUNANDTHROWABLE,
        GUNANDHANDHELD,
        GUNANDAMMO
    };

private:
    QString m_fileName{"Default file name"};
    const QString m_slot1Tag{R"(<Firearm SlotNumber = "0">)"};
    const QString m_slot2TagGun{R"(<Firearm SlotNumber = "1">)"};
    const QString m_slot2TagThrowable{R"(<ThrownItem SlotNumber = "1">)"};
    const QString m_slot2TagHandHeld{R"(<HandHeldItem SlotNumber = "1">)"};
    const QString m_itemFileNameTag{"<ItemFileName>"};
    QString m_slot1ItemFileName{"default.gun"};
    QString m_slot2ItemFileName{"default.gun"};
    const QString m_MagCountTag{"<MagazineCount>"};
    const QString m_ItemCountTag{"<Count>"};
    QString m_slot1MagCount{"0"};
    QString m_slot2MagCount{"0"};
    QString m_slot2ItemCount{"0"};
    const QString m_extraAmmoTag{"<ExtraAmmo>"};
    QString m_extraAmmo{"0"};
    //int m_itemCount{0};
    kitType m_type;

    fileReadResult findGameData(QString targetSlotTag, std::ifstream &kitFile);
    fileReadResult getGameData(QString targetSlotTag, QString targetTag, QString &valueToFill, std::ifstream &kitFile);

public:
    Kit(QString fileName, std::ifstream &kitFile);

    ~Kit()
    {
    }

    void print() const;
};

#endif // KIT_H
