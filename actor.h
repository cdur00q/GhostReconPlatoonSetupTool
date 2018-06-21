#ifndef ACTOR_H
#define ACTOR_H

#include "variables.h"
#include <QString>
#include <QTextStream> // for printing to console

class actor
{
private:
    const QString m_fileName{"Default file name"};
    const QString m_nameTag{"<ActorName>"};
    QString m_name{"Default name"};
    const QString m_kitPathTag{"<KitPath>"};
    QString m_kitPath{"rifleman"};
    const QString m_weaponStatTag{"<Weapon>"};
    QString m_weaponStat{"1"}; // value can only be 1-8 so okay to store as string
    const QString m_staminaStatTag{"<Stamina>"};
    QString m_staminaStat{"1"};
    const QString m_stealthStatTag{"<Stealth>"};
    QString m_stealthStat{"1"};
    const QString m_leadershipStatTag{"<Leadership>"};
    QString m_leadershipStat{"1"};

    fileReadResult getGameData(QString targetTag, QString &valueToFill, std::ifstream &actorFile);

public:
    actor(QString fileName, std::ifstream &actorFile);

    ~actor()
    {
    }

    QString getName(){return m_name;}

    void print()
    {
        QTextStream(stdout) << m_fileName <<
        " " << m_name <<
        " " << m_kitPath <<
        " W: " << m_weaponStat <<
        " S: " << m_staminaStat <<
        " S: " << m_stealthStat <<
        " L: " << m_leadershipStat << '\n';
    }
};

#endif // ACTOR_H
