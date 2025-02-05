#pragma once
#include "ScriptEnvironment.hpp"

class ModuleManifest;
struct BuildSetup;


class ModuleScript : public ScriptEnvironment
{
public:
    bool configureModule(ModuleManifest* module);
    bool configureSetup(const BuildSetup&, ModuleManifest* module);

protected:
    bool bindEnvironment() override;
};
