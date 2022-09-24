#pragma once
#include <mutex>

namespace Faro
{
    class Mutex
    {
    public:
        void Lock();
        void Unlock();

    private:
        std::mutex mutex;
    };

    template<class T>
    class ThreadSafe
    {
    public:
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

    private:
        Mutex mutex;

        T data;
    };
}
