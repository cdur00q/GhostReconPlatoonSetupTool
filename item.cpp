#include "item.h"

Item::Item(QString fileName, std::ifstream &itemFile, const Strings &strings)
    : Projectile(fileName, itemFile, strings)
{
}
