#include "ModuleA.hpp"

namespace Faro
{
    void ModuleA::Load()
    {
    }

    void ModuleA::Unload()
    {
    }

    String ModuleA::GetName()
    {
        return "ModuleA";
    }

    Array<String> ModuleA::GetRuntimeDependencies()
    {
        return { };
    }
}
