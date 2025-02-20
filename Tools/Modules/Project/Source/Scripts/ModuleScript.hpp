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
    ScriptObjectFunction(ScriptBuildSetup, getConfig);
    ScriptObjectFunction(ScriptBuildSetup, getTarget);

private:
    BuildSetup setup;
};

class ScriptModuleBase : public ScriptObject
{
public:
    ScriptModuleBase(const ModuleManifest* moduleManifest);
    ScriptObjectFunction(ScriptModuleBase, getDirectory);
    ScriptObjectFunction(ScriptModuleBase, addLinkerLibrary);
    ScriptObjectFunction(ScriptModuleBase, addIncludePrivate);
    ScriptObjectFunction(ScriptModuleBase, addIncludePublic);

    std::vector<std::string> linkerLibraries;
    std::vector<std::string> includesPrivate;
    std::vector<std::string> includesPublic;

protected:
    const ModuleManifest* moduleManifest = nullptr;
};

class ScriptModuleConfig : public ScriptModuleBase
{
public:
    ScriptModuleConfig(const ModuleManifest* moduleManifest);
    ScriptObjectFunction(ScriptModuleConfig, setName);
    ScriptObjectFunction(ScriptModuleConfig, addSource);
    ScriptObjectFunction(ScriptModuleConfig, scanSource);
    ScriptObjectFunction(ScriptModuleConfig, setType);
    ScriptObjectFunction(ScriptModuleConfig, addDependency);
    ScriptObjectFunction(ScriptModuleConfig, setSolutionLocation);

    std::string name = "";
    std::vector<std::string> sourceFiles;
    ModuleType type = MT_Library;
    std::vector<std::string> dependencies;
    std::string solutionPath = "";
};

class ScriptModuleSetup : public ScriptModuleBase
{
public:
    ScriptModuleSetup(const ModuleManifest* moduleManifest);
};

class ModuleScript : public ScriptEnvironment
{
public:
    bool configureModule(ScriptModuleConfig& module);
    bool configureSetup(ScriptBuildSetup& buildSetup, ScriptModuleSetup& buildModule);

protected:
    bool bindEnvironment() override;
};
