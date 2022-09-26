#pragma once
#include <ThreadSafety.hpp>
#include <Primitives.hpp>
#include <Containers/Array.hpp>
#include <Math/Time.hpp>

namespace Faro
{
    /**
     * @brief Sleep the calling thread by the given duration.
     * @param duration Amount of time to sleep
     */
    extern void Sleep(Duration duration);

    /// @brief Function type to be ran on another thread.
    typedef Function<void()> ThreadTask;

    /// @brief Abstract interface of a thread.
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

    /// @brief Base class for a thread.
    class IThread : public IThreadInterface
    {
    public:
        /// @brief Start the thread.
        void Start();

        /// @brief Request the thread to stop.
        void RequestStop();

        /// @brief Request the thread to stop and wait for it.
        void WaitForStop();

        /**
         * @brief Check if the thread is currently running.
         * @return true The thread is running
         * @return false The thread is not running
         */
        bool IsRunning();

    private:
        void ThreadRun();

        std::thread thread;

        ThreadSafe<bool> shouldRun = true;
        ThreadSafe<bool> isRunning = false;
    };
}
