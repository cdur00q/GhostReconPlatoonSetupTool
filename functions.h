#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"
#include "actor.h"
#include <vector>
#include <fstream>
#include <QString>

//void syncSelectionBoxes(const std::vector<actor> soldierPool, const int soldierPoolCurRow, const std::vector<actor*> fireteam, Ui::PlatoonSetup *ui);

int writeCoopAvatar(std::ofstream &file);

//fileReadResult getGameData(QString target, std::ifstream &actorFile, std::vector<actor> &actors);

#endif // FUNCTIONS_H
