#include "MainThread.hpp"
#include "Containers/String.hpp"

namespace Faro
{
    String MainThread::id = "MainThread";

    int MainThread::Run()
    {
        running = true;

        RegisterThread();

        if (!app.Init()) return -1;

        while (app.Update())
        {
            RunTasks();
        }

        if (!app.Destroy()) return -1;

        UnregisterThread();

        return 0;
    }
}
