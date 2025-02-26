#pragma once
#include <string>
#include "FaroToolsExports.generated.h"

struct MSVCVersion
{
    //TODO Don't use STL
    std::string redistVersion;
    std::string root;
};

extern FaroToolsExports unsigned int msvcInstallations();
extern FaroToolsExports MSVCVersion msvcInstallation(unsigned int index);
