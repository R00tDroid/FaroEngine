#pragma once
#include <string>
#include <vector>

struct MSVCVersion
{
    std::string redistVersion;
    std::string root;
};

extern const std::vector<MSVCVersion>& getMSVCInstallations();
