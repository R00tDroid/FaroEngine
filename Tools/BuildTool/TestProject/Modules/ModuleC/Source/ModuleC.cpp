#include "ModuleC.hpp"
#include <ModuleB.hpp>

namespace Faro
{
    void ModuleC::Load()
    {
    }

    void ModuleC::Unload()
    {
    }

    String ModuleC::GetName()
    {
        return "ModuleC";
    }

    Array<String> ModuleC::GetRuntimeDependencies()
    {
        return { "ModuleB" };
    }
}
