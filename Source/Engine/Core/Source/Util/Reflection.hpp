#pragma once
#include "ReflectionTypes.hpp"
#include "Containers/Array.hpp"
#include "Containers/Map.hpp"

namespace Faro
{
    class Reflection
    {
    public:
        bool init();

    private:
        Array<const ReflectedType*> reflectionTypes;
        Map<const char*, const ReflectedType*> reflectionNames;
    };

    extern Reflection GReflection;
}
