#include "ModuleB.hpp"
#include <Log.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsInterface.hpp>
#include <Assert.hpp>

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
