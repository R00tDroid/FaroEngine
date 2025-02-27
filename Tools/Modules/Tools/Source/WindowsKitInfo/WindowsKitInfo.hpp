#pragma once
#include <string>
#include "FaroToolsExports.generated.h"

//TODO No STL
struct WindowsKit
{
    std::string Root;
    std::string Version;
};

extern FaroToolsExports unsigned int windowsKits();
extern FaroToolsExports WindowsKit windowsKit(unsigned int index);
