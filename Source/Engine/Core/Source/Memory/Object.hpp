#pragma once

namespace Faro
{
    /// @brief Base class for dynamically allocated objects.
    class IObject
    {
    public:
        virtual ~IObject() = default;

        /// @brief Run the initialization logic of this object. Should be overriden by deriving classes.
        virtual void Init();

        /**
         * @brief Run the destruction logic of this class. This also frees its memory.
         * @warning When overriding, the super call must be performed **last**
         */
        virtual void Destroy();
    };
}
