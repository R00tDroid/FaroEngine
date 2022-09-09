#pragma once

namespace Faro
{
    class Application
    {
    public:
        Application();
        ~Application();

        int Run();

    private:
        bool Init();
        bool Update();
        bool Destroy();
    };

    extern Application* GApplication;
}
