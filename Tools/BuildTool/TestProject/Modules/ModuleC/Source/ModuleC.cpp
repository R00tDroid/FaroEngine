#include "ModuleC.hpp"
#include <Log.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsInterface.hpp>
#include <Assert.hpp>

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
