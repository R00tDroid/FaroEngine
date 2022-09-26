#pragma once

namespace Faro
{
    class Application
    {
    public:
        Application();
        ~Application();

        bool Init();
        bool Update();
        bool Destroy();
    };

    extern Application* GApplication;
}
