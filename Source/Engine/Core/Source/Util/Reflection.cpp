#include "Reflection.hpp"
#include "ReflectionTypes.hpp"
#include <vector>
#include "Log.hpp"

using namespace Faro;

Reflection Faro::GReflection;

extern std::vector<ReflectedType*> registerReflectionTypes();

LOG_DECLARATION(reflection, reflection)

bool Reflection::init()
{
    std::vector<ReflectedType*> types = registerReflectionTypes();
    for (ReflectedType* reflectionType : types)
    {
        reflectionTypes.Add(reflectionType);
        reflectionNames.Add(reflectionType->GetName(), reflectionType);
    }

    for (ReflectedType* reflectionType : reflectionTypes)
    {
        switch (reflectionType->GetTypeId())
        {
        case Class:
        {
            ReflectedClass* reflectedClass = static_cast<ReflectedClass*>(reflectionType);
            for (const char* baseClassName : reflectedClass->baseTypeNames)
            {
                if (reflectionNames.Contains(baseClassName))
                {
                    ReflectedClass* baseType = (ReflectedClass*)reflectionNames[baseClassName];
                    reflectedClass->baseTypes.Add(baseType);
                    baseType->childTypes.Add(reflectedClass);
                }
                else
                {
                    Logger::Log(reflection, LC_Debug, "Did not find base class %s for %s", baseClassName, reflectedClass->GetName());
                }
            }
            
            break;
        }
        default: { break; }
        }
    }

    return true;
}
