#include "ModuleA.hpp"
#include <Log.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsInterface.hpp>
#include <Assert.hpp>

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
