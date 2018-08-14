#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>
#include <QString>


constexpr int fireteamMaxMembers{3};
const QString actorExtension{".atr"};
const QString gunExtension{".gun"};
const QString projectileExtension{".prj"};
const QString itemExtension{".itm"};
const QString kitExtension{".kit"};
const QString classRifleman{"rifleman"}; // class as in soldier's skill/role, not c++ class
const QString classHeavyWeapons{"heavy-weapons"};
const QString classSniper{"sniper"};
const QString classDemolitions{"demolitions"};
const std::string mainGameDirectory{"C:\\Program Files (x86)\\Red Storm Entertainment\\Ghost Recon"};
// ***switch main game directory to realitive path for release build***

enum class fileReadResult
{
    FOUND,
    NOTFOUND,
    FILESTREAMERROR
};

#endif // VARIABLES_H
