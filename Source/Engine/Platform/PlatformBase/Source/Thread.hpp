#pragma once
#include <ThreadSafety.hpp>

namespace Faro
{
    class IThreadInterface
    {
    protected:
        virtual void ThreadInit() = 0;
        virtual void ThreadUpdate() = 0;
        virtual void ThreadDestroy() = 0;
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
