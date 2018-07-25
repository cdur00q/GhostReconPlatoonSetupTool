#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"
#include "actor.h"
#include "strings.h"
#include "gun.h"
#include "projectile.h"
#include "item.h"
#include <vector>
#include <fstream>
#include <QString>

long getRandomNumber(long min, long max);

QString getFileExtension(const QString &fileName);

void readInActors(const std::string &directoryPath, std::vector<Actor> &actors);

void readInGuns(const std::string &directoryPath, std::vector<Gun> &guns, const Strings &strings);

void readInProjectiles(const std::string &directoryPath, std::vector<Projectile> &projectiles, const Strings &strings);

void readInItems(const std::string &directoryPath, std::vector<Item> &items, const Strings &strings);

void assignRandomActorToVector(const std::vector<Actor> &source, std::vector<Actor> &destination);

int writeCoopAvatar(std::ofstream &file);

#endif // FUNCTIONS_H
