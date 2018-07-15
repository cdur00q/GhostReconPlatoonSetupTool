#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "variables.h"
#include "strings.h"
#include <QString>
#include <QTextStream> // for printing to console

class Projectile
{
private:
    QString m_fileName{"Default file name"};
    const QString m_nameTokenTag{"<NameToken>"};
    QString m_nameToken{"Default name Token"};
    QString m_name{"Default name"};

    // extracts 1 piece of data from a .prj file. targetTag is the data tag to look for
    fileReadResult getGameData(QString targetTag, QString &valueToFill, std::ifstream &projectileFile);

public:
    Projectile(QString fileName, std::ifstream &projectileFile, const Strings &strings);

    ~Projectile()
    {
    }

    QString getFileName() const {return m_fileName;}
    QString getName() const {return m_name;}

    void print() const;
};

#endif // PROJECTILE_H