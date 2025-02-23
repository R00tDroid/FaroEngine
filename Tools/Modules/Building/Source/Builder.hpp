#pragma once
#include "FaroBuildingExports.generated.h"
#include <BuildSetup.hpp>

class ProjectManifest;
class ModuleManifest;

namespace Builder
{
    bool FaroBuildingExports build(const BuildSetup& buildSetup, const ProjectManifest* project, unsigned int moduleCount, const ModuleManifest** modules);
};
