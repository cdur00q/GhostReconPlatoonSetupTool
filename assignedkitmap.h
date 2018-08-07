#ifndef ASSIGNEDKITMAP_H
#define ASSIGNEDKITMAP_H

#include <map>
#include <QString>
#include<QMessageBox>


class AssignedKitMap
{
private:
    struct KitInfo
    {
        QString fileName;
        int index;
    };
    std::map<QString, KitInfo> m_kitMap;

public:
    AssignedKitMap();

    ~AssignedKitMap()
    {
    }

    void assignKitToActor(const QString &kitFileName, int kitIndex, const QString& actorFileName);

    QString getKitFileName(const QString& actorFileName) const;

    int getKitIndex(const QString& actorFileName) const;
};

#endif // ASSIGNEDKITMAP_H
