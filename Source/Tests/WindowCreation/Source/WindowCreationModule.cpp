#include "WindowCreationModule.hpp"
#include <Log.hpp>


namespace Faro
{
    void WindowCreationModule::Load()
    {
        window = GPlatform->CreateWindow();
        window->SetTitle("Test window");
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
