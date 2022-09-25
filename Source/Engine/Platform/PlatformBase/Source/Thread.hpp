#pragma once
#include <ThreadSafety.hpp>
#include <Primitives.hpp>
#include <Containers/Array.hpp>

namespace Faro
{
    /// @brief Function type to be ran on another thread.
    typedef Function<void()> ThreadTask;

    /// @brief Abstract interface of a thread
    class IThreadInterface
    {
    public:
        virtual ~IThreadInterface() = default;

        /**
         * @brief Schedule the supplied task to be executed ont this thread.
         * @param task Task function to execute
         */
        void AddTask(ThreadTask task);

    protected:
        /// @brief Executed while starting the thread.
        virtual void ThreadInit() = 0;
        /// @brief Executed while the thread is active.
        virtual void ThreadUpdate() = 0;
        /// @brief Executed when the thread is being stopped.
        virtual void ThreadDestroy() = 0;

        /// @brief Executed any pending tasks.
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
