#pragma once
#include <ThreadSafety.hpp>
#include <Primitives.hpp>
#include <Containers/Array.hpp>

namespace Faro
{
    typedef Function<void()> ThreadTask;

    class IThreadInterface
    {
    public:
        virtual ~IThreadInterface() = default;

        void AddTask(ThreadTask task);

    protected:
        virtual void ThreadInit() = 0;
        virtual void ThreadUpdate() = 0;
        virtual void ThreadDestroy() = 0;

        void RunTasks();

    private:
        ThreadSafe<Array<ThreadTask>> tasks;
    };

    class IThread : public IThreadInterface
    {
    public:
        void Start();

        void RequestStop();

        void WaitForStop();

        bool IsRunning();

    private:
        void ThreadRun();

        std::thread thread;

        ThreadSafe<bool> shouldRun = true;
        ThreadSafe<bool> isRunning = false;
    };
}
