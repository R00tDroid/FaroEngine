#include "BuildSetup.hpp"
#include <cstring>

const char* configurationToString(Configuration configuration)
{
    switch (configuration)
    {
        case C_Debug: return "debug";
        case C_Development: return "development";
        case C_Release: return "release";
    }
    return nullptr;
}

Configuration configurationFromString(const char* name)
{
    if (strcmp(name, "debug") == 0) return C_Debug;
    if (strcmp(name, "development") == 0) return C_Development;
    if (strcmp(name, "release") == 0) return C_Release;
    return C_ENUMSIZE;
}
