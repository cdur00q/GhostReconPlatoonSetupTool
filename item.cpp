#include "item.h"

Item::Item(QString fileName, std::ifstream &itemFile)
    : Projectile(fileName, itemFile)
{
}
