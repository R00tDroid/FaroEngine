#include "ModuleScript.hpp"
#include "ModuleInfo.hpp"
#include "Utility.hpp"

class EcmaBuildSetup : public EcmaObject
{
public:
    EcmaBuildSetup(const BuildSetup&)
    {
        EcmaObjectFunctionBinding(config, 0);
        EcmaObjectFunctionBinding(target, 0);
    }

    EcmaObjectFunction(EcmaBuildSetup, config)
    {
        duk_push_string(context, "Release");
        return 1;
    }

    EcmaObjectFunction(EcmaBuildSetup, target)
    {
        duk_push_string(context, "WindowsX64");
        return 1;
    }
};

class EcmaModule : public EcmaObject
{
public:
    EcmaModule(ModuleManifest* moduleManifest) : moduleManifest(moduleManifest)
    {
        EcmaObjectFunctionBinding(dir, 0);
        EcmaObjectFunctionBinding(setName, 1);
        EcmaObjectFunctionBinding(scanSource, 1);
    }

    EcmaObjectFunction(EcmaModule, dir)
    {
        duk_push_string(context, moduleManifest->manifestDirectory());
        return 1;
    }

    EcmaObjectFunction(EcmaModule, setName)
    {
        name = duk_safe_to_string(context, 0);
        Utility::PrintLine("Name: " + name);
        return 0;
    }
    const std::string& getName() const { return name; }

    EcmaObjectFunction(EcmaModule, scanSource)
    {
        std::filesystem::path path(duk_safe_to_string(context, 0));
        path.make_preferred();
        path = weakly_canonical(path);

        Utility::PrintLine("Scan source: " + path.string());
        return 0;
    }

private:
    ModuleManifest* moduleManifest = nullptr;
    std::string name;
};

bool ModuleScript::configure(const BuildSetup& setup, ModuleManifest* module)
{
    duk_push_global_object(context);

    EcmaBuildSetup buildSetup(setup);
    EcmaModule buildModule(module);

    if (duk_get_global_string(context, "configure") == 0)
    {
        Utility::PrintLine("Could not find function 'configure'");
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
