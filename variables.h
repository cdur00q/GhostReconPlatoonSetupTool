#ifndef VARIABLES_H
#define VARIABLES_H

#include <string>

struct actor
{
    std::string name;
};

enum class fileReadResult
{
    FOUND,
    NOTFOUND,
    FILESTREAMERROR
};

#endif // VARIABLES_H
