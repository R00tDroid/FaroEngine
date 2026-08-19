#include "Reflection.hpp"
#include "ReflectionTypes.hpp"
#include <vector>

extern std::vector<const ReflectedType*> registerReflectionTypes();

bool initReflection()
{
    std::vector<const ReflectedType*> types = registerReflectionTypes();

    //TODO Link child and parent types

    return true;
}
