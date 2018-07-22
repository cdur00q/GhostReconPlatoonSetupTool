#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"
#include "actor.h"
#include <vector>
#include <fstream>
#include <QString>

long getRandomNumber(long min, long max);

QString getFileExtension(const QString &fileName);

void readInActors(const std::string &directoryPath, std::vector<Actor> &actors);

void assignRandomActorToVector(const std::vector<Actor> &source, std::vector<Actor> &destination);

int writeCoopAvatar(std::ofstream &file);

#endif // FUNCTIONS_H
