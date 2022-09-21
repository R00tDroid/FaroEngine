#pragma once
#include <Containers/Array.hpp>
#include <Containers/String.hpp>
#include <Util/ClassRegistry.hpp>

namespace Faro
{
    class IModule
    {
    public:
        virtual void Load() = 0;
        virtual void Unload() = 0;
        virtual String GetName() = 0;
        virtual Array<String> GetRuntimeDependencies() = 0;

        uint32 loadOrder = 0;
    };

    extern Array<IModule*> GetRegisteredModules();

    DEFINE_INSTANCE_REGISTRY(Module, Faro::IModule)
#define REGISTER_MODULE(MODULE) REGISTER_INSTANCE(Module, MODULE)
}
