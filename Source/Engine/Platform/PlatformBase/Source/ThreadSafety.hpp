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

    /// @brief Container class providing thread-safe access to the internal object.
    /// @tparam T Type of the contained data object
    template<class T>
    class ThreadSafe
    {
    public:
        ThreadSafe() : data() { }
        ThreadSafe(T initialValue) : data(initialValue) { }

        /// @brief Gain exclusive access to this object.
        void Lock()
        {
            mutex.Lock();
        }

        /// @brief Release exclusive access to this object.
        void Unlock()
        {
            mutex.Unlock();
        }

        /**
         * @brief Set the value of this object.
         * @param value New value of the object
         */
        void Set(T value)
        {
            Lock();
            data = value;
            Unlock();
        }

        /**
         * @brief Get a copy of the contained value.
         * @return T Copy of the contained value
         */
        T GetCopy()
        {
            Lock();
            T returnValue = data;
            Unlock();

            return returnValue;
        }

        /**
         * @brief Get a reference to the contained value.
         * @warning Must be locked with #Lock and unlocked with #Unlock
         * @return T& Reference to the contained value
         */
        T& Get()
        {
            return data;
        }

/**
         * @brief Get a pointer to the contained value.
         * @warning Must be locked with #Lock and unlocked with #Unlock
         * @return T* Pointer to the contained value
         */
        T* operator-> ()
        {
            return &data;
        }

    private:
        Mutex mutex;

        T data;
    };
}
