#ifndef ITEM_H
#define ITEM_H

#include "projectile.h"


class Item : public Projectile
{
public:
    Item(QString fileName, std::ifstream &itemFile);

    ~Item()
    {
    }
};

#endif // ITEM_H
