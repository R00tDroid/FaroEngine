#include "ClassRegistry.hpp"

namespace Faro
{       
    ReflectionManager& ReflectionManager::Get()
    {
        static ReflectionManager instance;
        return instance;
    }
}
