#include "Application.h"

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
        return true;
    }

    bool Application::Update()
    {
        return true;
    }

    bool Application::Destroy()
    {
        return true;
    }
}
