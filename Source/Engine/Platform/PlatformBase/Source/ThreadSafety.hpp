#pragma once
#include <mutex>

namespace Faro
{
    /// @brief Synchronization flag to gain exclusive access on the calling thread.
    class Mutex
    {
    public:
        /// @brief Obtain exclusive access to this mutex.
        void Lock();

        /// @brief Release the previously claimed access.
        void Unlock();

    private:
        std::mutex mutex;
    };

    template<class T>
    class ThreadSafe
    {
    public:
        ThreadSafe() : data() { }
        ThreadSafe(T initialValue) : data(initialValue) { }

        void Lock()
        {
            mutex.Lock();
        }

        void Unlock()
        {
            mutex.Unlock();
        }

        void Set(T value)
        {
            Lock();
            data = value;
            Unlock();
        }

        T GetCopy()
        {
            Lock();
            T returnValue = data;
            Unlock();

            return returnValue;
        }

        T& Get()
        {
            return data;
        }

        T* operator-> ()
        {
            return &data;
        }

    private:
        Mutex mutex;

        T data;
    };
}
