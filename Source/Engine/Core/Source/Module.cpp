#include "Module.h"

namespace Faro {
    IMPLEMENT_INSTANCE_REGISTRY(Module, Faro::IModule)
}

Faro::Array<Faro::IModule*> Faro::GetRegisteredModules()
{
    return InstanceRegistry_Module::Get().GetValues();
}
