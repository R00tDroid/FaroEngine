#pragma once
#include <functional>
#include "../Containers/Array.hpp"

namespace Faro
{
    template<class ...Args>
    class Broadcast
    {
    public:
        void Add(Function<void(Args...)> function)
        {
            boundFunctions.Add(function);
        }

        void Dispatch(Args... arguments)
        {
            for (Function<void(Args...)>& function : boundFunctions)
            {
                function(boundFunctions...);
            }
        }

    private:
        Array<Function<void(Args...)>> boundFunctions;
    };
}
