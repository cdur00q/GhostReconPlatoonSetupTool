#ifndef ITEM_H
#define ITEM_H

#include "projectile.h"


class Item : public Projectile
{
public:
    Item(QString fileName, std::ifstream &itemFile, const Strings &strings);

    ~Item()
    {
    }
};

#endif // ITEM_H
