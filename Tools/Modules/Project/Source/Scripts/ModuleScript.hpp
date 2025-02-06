#pragma once
#include "BuildSetup.hpp"
#include "ModuleInfo.hpp"
#include "ScriptEnvironment.hpp"

class ModuleManifest;
struct BuildSetup;

class ScriptBuildSetup : public ScriptObject
{
public:
    ScriptBuildSetup(const BuildSetup& setup);
    ScriptObjectFunction(ScriptBuildSetup, config);
    ScriptObjectFunction(ScriptBuildSetup, target);

private:
    BuildSetup setup;
};

class ScriptModuleBase : public ScriptObject
{
public:
    ScriptModuleBase(ModuleManifest* moduleManifest);
    ScriptObjectFunction(ScriptModuleBase, dir);
    ScriptObjectFunction(ScriptModuleBase, source);
    ScriptObjectFunction(ScriptModuleBase, addDependency);
    ScriptObjectFunction(ScriptModuleBase, setType);
    ScriptObjectFunction(ScriptModuleBase, setSolutionLocation);
    ScriptObjectFunction(ScriptModuleBase, link);

protected:
    ModuleManifest* moduleManifest = nullptr;
};

class ScriptModuleConfig : public ScriptModuleBase
{
public:
    ScriptModuleConfig(ModuleManifest* moduleManifest);
    ScriptObjectFunction(ScriptModuleConfig, setName);
    ScriptObjectFunction(ScriptModuleConfig, addSource);
    ScriptObjectFunction(ScriptModuleConfig, scanSource);
    ScriptObjectFunction(ScriptModuleConfig, addIncludePrivate);
    ScriptObjectFunction(ScriptModuleConfig, addIncludePublic);
};

class ModuleScript : public ScriptEnvironment
{
public:
    bool configureModule(ModuleManifest* module);
    bool configureSetup(const BuildSetup&, ModuleManifest* module);

protected:
    bool bindEnvironment() override;
};
