#include "Thread.hpp"

namespace Faro
{
    void Sleep(Duration duration)
    {
        std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::microseconds(duration.TotalMicroseconds())));
    }

    void IThreadInterface::AddTask(ThreadTask task)
    {
        tasks.Lock();
        tasks->Add(task);
        tasks.Unlock();
    }

    void IThreadInterface::RunTasks()
    {
        tasks.Lock();
        for (ThreadTask& task : tasks.Get())
        {
            task();
        }
        tasks->Clear();
        tasks.Unlock();
    }

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
            RunTasks();
            ThreadUpdate();
        }

        ThreadDestroy();

        isRunning.Set(false);
    }
}
