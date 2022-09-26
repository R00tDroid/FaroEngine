#include "Application.hpp"
#include <Log.hpp>
#include "ModuleManager.hpp"
#include "EngineVersion.hpp"

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

    bool Application::Init()
    {
        Log(AppLog, LC_Info, "Faro Engine %s", GEngineVersion.GetFullVersion().Data());
        Log(AppLog, LC_Debug, "Initializing");

        if (!GModuleManager->Init()) return false;

        Log(AppLog, LC_Debug, "Entering main loop");

        return true;
    }

    bool Application::Update()
    {
        if (!GModuleManager->Update()) return false;

        return true;
    }

    bool Application::Destroy()
    {
        Log(AppLog, LC_Debug, "Starting destruction");

        if (!GModuleManager->Destroy()) return false;

        return true;
    }
}
