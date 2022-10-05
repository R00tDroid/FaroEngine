#include "Thread.hpp"

namespace Faro
{
    void Sleep(Duration duration)
    {
        std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::microseconds(duration.TotalMicroseconds())));
    }

    ThreadSafe<Array<IThreadInterface*>> registeredThread;

    void RunOnThread(String threadId, ThreadTask task)
    {
        if (threadId.Empty()) return;

        registeredThread.Lock();
        for (IThreadInterface* thread : registeredThread.Get())
        {
            if (thread->GetThreadId() == threadId)
            {
                thread->AddTask(task);
                break;
            }
        }
        registeredThread.Unlock();
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

    void IThreadInterface::RegisterThread()
    {
        registeredThread.Lock();
        registeredThread->Add(this);
        registeredThread.Unlock();
    }

    void IThreadInterface::UnregisterThread()
    {
        registeredThread.Lock();
        registeredThread->Remove(this);
        registeredThread.Unlock();
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

    String IThread::GetThreadId()
    {
        return String();
    }

    void IThread::ThreadRun()
    {
        isRunning.Set(true);

        RegisterThread();

        ThreadInit();

        while (shouldRun.GetCopy())
        {
            RunTasks();
            ThreadUpdate();
        }

        ThreadDestroy();

        UnregisterThread();

        isRunning.Set(false);
    }
}
