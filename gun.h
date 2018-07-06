#ifndef GUN_H
#define GUN_H

#include "variables.h"
#include <QString>
#include <QTextStream> // for printing to console

class Gun
{
private:
    QString m_fileName{"Default file name"};
    const QString m_nameTokenTag{"<NameToken>"};
    QString m_nameToken{"Default name Token"};
    const QString m_magCapTag{"<MagazineCapacity>"};
    QString m_magCap{"0"};
    const QString m_maxRangeTag{"<MaxRange>"};
    QString m_maxRange{"0.0"};
    const QString m_muzzleVelocityTag{"<VelocityCoefficient0>"};
    QString m_muzzleVelocity{"0.0"};
    const QString m_recoilTag{"<Recoil>"};
    QString m_recoil{"0"};
    const QString m_maxAccuracyTag{"<StationaryProneAccuracy>"}; // laying prone is the most accurate shot you can make in the game
    QString m_maxAccuracy{"0.0"};
    const QString m_stabilizationTimeTag{"<StabilizationTime>"};
    QString m_stabilizationTime{"0.0"};
    const QString m_silencedTag{"<Silenced>"};
    QString m_silenced{"0"};
    struct FireMode
    {
        QString rpm;
        QString mode;
    };
    std::vector<FireMode> fireModes;
    QString m_maxZoom{"0"};

    // extracts 1 piece of data from a .gun file. targetTag is the data tag to look for
    fileReadResult getGameData(QString targetTag, QString &valueToFill, std::ifstream &gunFile);
    // uses recursion to extract all listed fire selection options from a .gun file
    fileReadResult getFireModes(std::vector<FireMode> &fireModes, std::ifstream &gunFile, int startReadingPos);
    // uses recursion to find and extract the maximum zoom value from a .gun file
    fileReadResult getMaxZoom(QString &valueToFill, std::ifstream &gunFile, int startReadingPos);

public:
    Gun(QString fileName, std::ifstream &gunFile);

    ~Gun()
    {
    }

    void print() const;
};

#endif // GUN_H
