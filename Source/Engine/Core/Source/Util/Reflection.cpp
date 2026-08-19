#include "Reflection.hpp"
#include "ReflectionTypes.hpp"
#include <vector>

using namespace Faro;

Reflection Faro::GReflection;

extern std::vector<const ReflectedType*> registerReflectionTypes();

bool Reflection::init()
{
    std::vector<const ReflectedType*> types = registerReflectionTypes();
    for (const ReflectedType* reflectionType : types)
    {
        reflectionTypes.Add(reflectionType);
        reflectionNames.Add(reflectionType->GetName(), reflectionType);
    }

    //TODO Link child and parent types

    return true;
}
