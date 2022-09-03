#include "./Module.h"

namespace Faro {
    IMPLEMENT_INSTANCE_REGISTRY(Module, Faro::Module)
}

Faro::Array<Faro::Module*> Faro::GetRegisteredModules()
{
    return InstanceRegistry_Module::Get().GetValues();
}

