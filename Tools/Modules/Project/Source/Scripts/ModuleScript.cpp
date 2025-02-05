#include "ModuleScript.hpp"
#include "ModuleInfo.hpp"
#include "Utility.hpp"

class ScriptBuildSetup : public ScriptObject
{
public:
    ScriptBuildSetup(const BuildSetup& setup) : setup(setup)
    {
        ScriptObjectFunctionBinding(config, 0);
        ScriptObjectFunctionBinding(target, 0);
    }

    ScriptObjectFunction(ScriptBuildSetup, config)
    {
        duk_push_string(context, configurationToString(setup.configuration));
        return 1;
    }

    ScriptObjectFunction(ScriptBuildSetup, target)
    {
        duk_push_string(context, setup.target->identifier());
        return 1;
    }

private:
    BuildSetup setup;
};

class ScriptModuleBase : public ScriptObject
{
public:
    ScriptModuleBase(ModuleManifest* moduleManifest) : moduleManifest(moduleManifest)
    {
        ScriptObjectFunctionBinding(dir, 0);
    }

    ScriptObjectFunction(ScriptModuleBase, dir)
    {
        duk_push_string(context, moduleManifest->manifestDirectory());
        return 1;
    }

protected:
    ModuleManifest* moduleManifest = nullptr;
};

class ScriptModuleConfig : public ScriptModuleBase
{
public:
    ScriptModuleConfig(ModuleManifest* moduleManifest) : ScriptModuleBase(moduleManifest)
    {
        ScriptObjectFunctionBinding(setName, 1);
        ScriptObjectFunctionBinding(scanSource, 1);
    }

    ScriptObjectFunction(ScriptModuleConfig, setName)
    {
        name = duk_safe_to_string(context, 0);
        Utility::PrintLine("Name: " + name);
        return 0;
    }
    const std::string& getName() const { return name; }

    ScriptObjectFunction(ScriptModuleConfig, scanSource)
    {
        std::filesystem::path path(duk_safe_to_string(context, 0));
        path.make_preferred();
        path = weakly_canonical(path);

        Utility::PrintLine("Scan source: " + path.string());
        return 0;
    }

protected:
    std::string name;
};

class ScriptModuleSetup : public ScriptModuleBase
{
public:
    ScriptModuleSetup(ModuleManifest* moduleManifest) : ScriptModuleBase(moduleManifest) { }   
};

bool ModuleScript::configureModule(ModuleManifest* module)
{
    duk_push_global_object(context);

    ScriptModuleConfig buildModule(module);

    if (duk_get_global_string(context, "configureModule") == 0)
    {
        Utility::PrintLine("Could not find function 'configureModule'");
        return false;
    }

    buildModule.bind(context);

    if (duk_pcall(context, 1) != DUK_EXEC_SUCCESS)
    {
        Utility::PrintLine("> " + std::string(duk_safe_to_string(context, -1)));
        return false;
    }

    return true;
}

bool ModuleScript::configureSetup(const BuildSetup& setup, ModuleManifest* module)
{
    duk_push_global_object(context);

    ScriptBuildSetup buildSetup(setup);
    ScriptModuleSetup buildModule(module);

    if (duk_get_global_string(context, "configureSetup") == 0)
    {
        Utility::PrintLine("Could not find function 'configureSetup'");
        return false;
    }

    buildSetup.bind(context);
    buildModule.bind(context);

    if (duk_pcall(context, 2) != DUK_EXEC_SUCCESS)
    {
        Utility::PrintLine("> " + std::string(duk_safe_to_string(context, -1)));
        return false;
    }

    return true;
}

bool ModuleScript::bindEnvironment()
{
    if (!ScriptEnvironment::bindEnvironment()) return false;

    return true;
}
