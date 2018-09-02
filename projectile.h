#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "variables.h"
#include "strings.h"
#include <fstream>
#include <QString>

class Projectile
{
private:
    QString m_fileName{"Default file name"};
    const QString m_nameTokenTag{"<NameToken>"};
    QString m_nameToken{"Default name Token"};
    QString m_name{"Default name"};

    // reads and stores one item of game data from aa projectile file
    fileReadResult getGameData(const QString &targetTag, QString &valueToFill, std::ifstream &projectileFile);

public:
    Projectile(QString fileName, std::ifstream &projectileFile, const Strings &strings);

    ~Projectile()
    {
    }

    const QString& getFileName() const {return m_fileName;}
    const QString& getName() const {return m_name;}

    Projectile& operator= (const Projectile &projectile);

    void print() const;
};

#endif // PROJECTILE_H
