#ifndef GUN_H
#define GUN_H

#include "variables.h"
#include "strings.h"
#include <vector>
#include <QString>

class Gun
{
private:
    QString m_fileName{"Default file name"};
    const QString m_nameTokenTag{"<NameToken>"};
    QString m_nameToken{"Default name Token"};
    QString m_name{"Default name"};
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
    const QString m_weaponTypeTag{"<WeaponMotionType>"};
    QString m_weaponType{"0"};
    const QString m_silencedTag{"<Silenced>"};
    QString m_silenced{"0"};
    struct FireMode
    {
        QString rpm;
        QString mode;
    };
    std::vector<FireMode> m_fireModes;
    QString m_maxZoom{"0"};

    // reads and stores one item of game data from a gun file
    fileReadResult getGameData(const QString &targetTag, QString &valueToFill, std::ifstream &gunFile);
    // uses recursion to extract all listed fire selection options from a gun file
    fileReadResult getFireModes(std::vector<FireMode> &fireModes, std::ifstream &gunFile, int startReadingPos);
    // uses recursion to find and extract the maximum zoom value from a gun file
    fileReadResult getMaxZoom(QString &valueToFill, std::ifstream &gunFile, int startReadingPos);

public:
    Gun(QString fileName, std::ifstream &gunFile);

    ~Gun()
    {
    }

    const QString& getFileName() const {return m_fileName;}
    const QString& getName() const {return m_name;}
    const QString& getMagCap() const {return m_magCap;}
    const QString& getMaxRange() const {return m_maxRange;}
    const QString& getMuzzleVelocity() const {return m_muzzleVelocity;}
    const QString& getRecoil() const {return m_recoil;}
    const QString& getMaxAccuracy() const {return m_maxAccuracy;}
    const QString& getStabilizationTime() const {return m_stabilizationTime;}
    const QString& getWeaponType() const {return m_weaponType;}
    const QString& getSilenced() const {return m_silenced;}
    const QString& getMaxZoom() const {return m_maxZoom;}
    const std::vector<FireMode>& getFireModes() const {return m_fireModes;}

    void setNameFromStrings(const Strings &strings) {m_name = strings.getString(m_nameToken);}

    Gun& operator= (const Gun &gun);

    void print() const;
};

#endif // GUN_H
