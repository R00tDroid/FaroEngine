#pragma once
#include "FaroReflectionExports.generated.h"

class ProjectManifest;
class ModuleManifest;

namespace Reflector
{
    bool FaroReflectionExports generateFileReflection(const ModuleManifest*, unsigned int fileIndex);
    bool FaroReflectionExports generateModuleReflection(const ModuleManifest*);
};
