#ifndef ACTOR_H
#define ACTOR_H

#include "variables.h"
#include <QString>
#include <QTextStream> // for printing to console

class Actor
{
private:
    QString m_fileName{"Default file name"};
    const QString m_nameTag{"<ActorName>"};
    QString m_name{"Default name"};
    const QString m_classNameTag{"<ClassName>"};
    QString m_className{"Default class name"};
    const QString m_kitPathTag{"<KitPath>"};
    QString m_kitPath{"rifleman"};
    const QString m_weaponStatTag{"<Weapon>"};
    QString m_weaponStat{"1"};
    const QString m_staminaStatTag{"<Stamina>"};
    QString m_staminaStat{"1"};
    const QString m_stealthStatTag{"<Stealth>"};
    QString m_stealthStat{"1"};
    const QString m_leadershipStatTag{"<Leadership>"};
    QString m_leadershipStat{"1"};

    fileReadResult getGameData(const QString &targetTag, QString &valueToFill, std::ifstream &actorFile);

public:
    Actor(QString fileName, std::ifstream &actorFile);

    ~Actor()
    {
    }

    const QString& getFileName() const {return m_fileName;}
    const QString& getName() const {return m_name;}
    const QString& getClassName() const {return m_className;}
    QString getFirstInitialLastName() const;
    const QString& getKitPath() const {return m_kitPath;}
    const QString& getWeaponStat() const {return m_weaponStat;}
    const QString& getStaminaStat() const {return m_staminaStat;}
    const QString& getStealthStat() const {return m_stealthStat;}
    const QString& getLeadershipStat() const {return m_leadershipStat;}

    friend bool operator== (const Actor &actor1, const Actor &actor2);
    friend bool operator!= (const Actor &actor1, const Actor &actor2);

    Actor& operator= (const Actor &actor);

    void print() const;
};

#endif // ACTOR_H
