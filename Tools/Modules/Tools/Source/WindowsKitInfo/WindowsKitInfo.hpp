#pragma once
#include <string>
#include "FaroToolsExports.generated.h"

//TODO No STL
struct WindowsKit
{
    std::string root;
    std::string version;
};

extern FaroToolsExports unsigned int windowsKits();
extern FaroToolsExports WindowsKit windowsKit(unsigned int index);
