#pragma once
#include <string>
#include <vector>

struct WindowsKit
{
    std::string Root;
    std::string Version;
};

extern const std::vector<WindowsKit>& GetWindowsKits();
