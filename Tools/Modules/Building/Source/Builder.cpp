#include "Builder.hpp"
#include "Utility.hpp"

bool Builder::build(const BuildSetup& buildSetup, const ProjectManifest*, unsigned int, const ModuleManifest**)
{
    if (buildSetup.target == nullptr)
    {
        Utility::PrintLine("Invalid build target");
        return false;
    }

    const Toolchain* toolchain = buildSetup.target->toolchain();
    if (toolchain == nullptr)
    {
        Utility::PrintLine("Invalid toolchain for build target");
        return false;
    }

    return false;
}
