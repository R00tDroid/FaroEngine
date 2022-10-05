#pragma once
#include "Application.hpp"
#include "Thread.hpp"

namespace Faro
{
    class MainThread : public IThreadInterface
    {
    public:
        static String id;

        String GetThreadId() override { return id; }

        int Run();

    protected:
        void ThreadInit() override {}
        void ThreadUpdate() override {}
        void ThreadDestroy() override {}

    private:
        Application app;
        bool running = false;
    };
}
