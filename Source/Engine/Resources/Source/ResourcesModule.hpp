#pragma once
#include <Module.hpp>
#include <Log.hpp>

namespace Faro
{
    LOG_DEFINITION(ResourceLog)

    Reflect() class ResourcesModule : public IModule
    {
    public:
        void Load() override;
        void Unload() override;
        String GetName() override;
        Array<String> GetRuntimeDependencies() override;
    };
}
