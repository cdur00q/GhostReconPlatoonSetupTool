#include "assignedkitmap.h"

#include <map>
#include <QString>
#include <QMessageBox>

AssignedKitMap::AssignedKitMap()
{

}

// adds a new actor/kit pair to the kit map or updates the kit info if the actor already exists in the kit map
void AssignedKitMap::assignKitToActor(const QString &kitFileName, int kitIndex, const QString& actorFileName)
{
    auto tempIterator{m_kitMap.find(actorFileName)}; // first check if this actor already exists in the kit map
    if (tempIterator != m_kitMap.end()) // it does, so update with passed in info
    {
        tempIterator->second = {kitFileName, kitIndex};
    }
    else // it doesn't, so add in this new actor
    {
        m_kitMap.insert(std::make_pair(actorFileName, KitInfo{kitFileName, kitIndex}));
    }
}

// returns the file name of the kit paired with the passed in actor
QString AssignedKitMap::getKitFileName(const QString& actorFileName) const
{
    auto tempIterator{m_kitMap.find(actorFileName)};
    if (tempIterator != m_kitMap.end())
    {
        return tempIterator->second.fileName;
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in AssignedKitMap::getKitFileName().  Actor not found.");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}

// returns the index of the kit paired with the passed in actor
int AssignedKitMap::getKitIndex(const QString& actorFileName) const
{
    auto tempIterator{m_kitMap.find(actorFileName)};
    if (tempIterator != m_kitMap.end())
    {
        return tempIterator->second.index;
    }
    else
    {
        QMessageBox msgBox(QMessageBox::Critical, "Error", "Error in AssignedKitMap::getKitIndex().  Actor not found.");
        msgBox.exec();
        exit(EXIT_FAILURE);
    }
}
