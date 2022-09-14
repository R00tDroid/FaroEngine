#include "Application.h"
#include "ModuleManager.h"

namespace Faro
{
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
        if (!Init())
        {
            return -1;
        }

        while (Update()) {}

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
