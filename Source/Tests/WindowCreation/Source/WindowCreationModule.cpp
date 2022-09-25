#include "WindowCreationModule.hpp"
#include <Log.hpp>

namespace Faro
{
    void WindowCreationModule::Load()
    {

    }

    void WindowCreationModule::Unload()
    {
    }

    String WindowCreationModule::GetName()
    {
        return "WindowCreation test";
    }

    Array<String> WindowCreationModule::GetRuntimeDependencies()
    {
        return { "Platform" };
    }
}
