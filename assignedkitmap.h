#ifndef ASSIGNEDKITMAP_H
#define ASSIGNEDKITMAP_H

#include <map>
#include <QString>

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

    // adds a new actor/kit pair to the kit map or updates the kit info if the actor already exists in the kit map
    void assignKitToActor(const QString &kitFileName, int kitIndex, const QString& actorFileName);

    // returns the file name of the kit paired with the passed in actor
    QString getKitFileName(const QString& actorFileName) const;

    // returns the index of the kit paired with the passed in actor
    int getKitIndex(const QString& actorFileName) const;
};

#endif // ASSIGNEDKITMAP_H
