#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>

// obsolete compared to actor class?
/*
struct actor
{
    std::string name;
};
*/

constexpr int fireteamMaxMembers{3};

enum class uiLists
{
    NONE,
    SOLDIERPOOL,
    ALPHA,
    BRAVO,
    CHARLIE
};

enum class fileReadResult
{
    FOUND,
    NOTFOUND,
    FILESTREAMERROR
};

#endif // VARIABLES_H
