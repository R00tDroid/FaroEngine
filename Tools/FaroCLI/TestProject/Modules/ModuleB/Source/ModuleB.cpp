#include "ModuleB.hpp"

namespace Faro
{
    void ModuleB::Load()
    {
    }

    void ModuleB::Unload()
    {
    }

    String ModuleB::GetName()
    {
        return "ModuleB";
    }

    Array<String> ModuleB::GetRuntimeDependencies()
    {
        return { "ModuleA" };
    }
}
