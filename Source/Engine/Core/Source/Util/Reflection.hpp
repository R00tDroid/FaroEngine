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
        Array<ReflectedType*> reflectionTypes;
        Map<const char*, ReflectedType*> reflectionNames;
    };

    extern Reflection GReflection;
}
