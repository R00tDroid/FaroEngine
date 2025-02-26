#include "Toolchain.hpp"
#include <vector>
#include "ToolchainMSVC.hpp"

std::vector<Toolchain*> toolchainList()
{
    return std::vector<Toolchain*> {
        &ToolchainMSVC::instance()
    };
}

unsigned int Toolchain::toolchains()
{
    return static_cast<unsigned int>(toolchainList().size());
}

Toolchain* Toolchain::toolchain(unsigned int index)
{
    return toolchainList()[index];
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

