#pragma once
#include <string>
#include <vector>

struct MSVCVersion
{
    std::string RedistVersion;
    std::string Root;
};

extern const std::vector<MSVCVersion>& GetMSVCInstallations();
