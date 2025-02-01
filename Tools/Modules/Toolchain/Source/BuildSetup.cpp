#include "BuildSetup.hpp"
#include <cstring>
#include <vector>
#include <Toolchain.hpp>

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

std::vector<BuildSetup> buildSetupList;
void getBuildSetups()
{
    if (!buildSetupList.empty()) return;

    unsigned int toolchainCount = Toolchain::toolchains();
    for (unsigned int toolchainIndex = 0; toolchainIndex < toolchainCount; toolchainIndex++)
    {
        Toolchain* toolchain = Toolchain::toolchain(toolchainIndex);
        unsigned int targetCount = toolchain->targets();
        for (unsigned int targetIndex = 0; targetIndex < targetCount; targetIndex++)
        {
            Target* target = toolchain->target(toolchainIndex);
            for (unsigned int configIndex = 0; configIndex < Configuration::C_ENUMSIZE; configIndex++)
            {
                buildSetupList.push_back({
                    static_cast<Configuration>(configIndex),
                    target
                });
            }
        }
    }
}

unsigned int buildSetups()
{
    getBuildSetups();
    return static_cast<unsigned int>(buildSetupList.size());
}

BuildSetup buildSetup(unsigned int index)
{
    getBuildSetups();
    return buildSetupList[index];
}
