#include "WindowCreationModule.hpp"
#include <Log.hpp>


namespace Faro
{
    void WindowCreationModule::Load()
    {
        window = GPlatform->CreateWindow();
    }

    void WindowCreationModule::Unload()
    {
        window->Destroy();
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
