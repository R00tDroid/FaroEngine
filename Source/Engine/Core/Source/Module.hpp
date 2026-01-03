#pragma once
#include <Containers/Array.hpp>
#include <Containers/String.hpp>
#include <Util/ClassRegistry.hpp>

namespace Faro
{
    Class() IModule
    {
    //IModule_BODY()
    public:
        virtual void Load() = 0;
        virtual void Unload() = 0;
        virtual String GetName() = 0;
        virtual Array<String> GetRuntimeDependencies() = 0;

        uint32 loadOrder = 0;

        Property()
        int test;
    };

    extern Array<IModule*> GetRegisteredModules();
}
