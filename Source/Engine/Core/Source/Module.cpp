#include "Module.hpp"

namespace Faro {
}

Faro::Array<Faro::IModule*> Faro::GetRegisteredModules()
{
    return {};// InstanceRegistry_Module::Get().GetValues();
}
