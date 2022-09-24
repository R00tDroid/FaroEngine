#include "Thread.hpp"

namespace Faro
{
    void IThread::Start()
    {
        shouldRun.Set(true);

        thread = std::thread(&IThread::ThreadRun, this);
        thread.detach();
    }

    void IThread::RequestStop()
    {
        shouldRun.Set(false);
    }

    void IThread::WaitForStop()
    {
        RequestStop();
        while (IsRunning()) { }
    }

    bool IThread::IsRunning()
    {
        return isRunning.GetCopy();
    }

    void IThread::ThreadRun()
    {
        isRunning.Set(true);

        ThreadInit();

        while (shouldRun.GetCopy())
        {
            ThreadUpdate();
        }

        ThreadDestroy();

        isRunning.Set(false);
    }
}
