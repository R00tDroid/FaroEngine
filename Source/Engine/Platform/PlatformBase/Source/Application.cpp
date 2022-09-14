#include "Application.hpp"
#include <Log.hpp>
#include "ModuleManager.hpp"

namespace Faro
{
    LOG_DECLARATION(AppLog, Application)

    Application* GApplication = nullptr;

    Application::Application()
    {
        GApplication = this;
    }

    Application::~Application()
    {
        GApplication = nullptr;
    }

    int Application::Run()
    {
        Log(AppLog, LC_Info, "Faro Engine");

        Log(AppLog, LC_Debug, "Initializing");
        if (!Init())
        {
            return -1;
        }

        Log(AppLog, LC_Debug, "Entering main loop");
        while (Update()) {}

        Log(AppLog, LC_Debug, "Starting destruction");
        if (!Destroy())
        {
            return -1;
        }

        return 0;
    }

    bool Application::Init()
    {
        if (!GModuleManager->Init()) return false;

        return true;
    }

    bool Application::Update()
    {
        if (!GModuleManager->Update()) return false;

        return true;
    }

    bool Application::Destroy()
    {
        if (!GModuleManager->Destroy()) return false;

        return true;
    }
}
