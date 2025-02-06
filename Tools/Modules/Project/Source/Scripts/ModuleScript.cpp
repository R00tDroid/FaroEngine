#include "ModuleScript.hpp"
#include "ModuleInfo.hpp"
#include "Utility.hpp"

class ScriptModuleSetup : public ScriptModuleBase
{
public:
    ScriptModuleSetup(ModuleManifest* moduleManifest) : ScriptModuleBase(moduleManifest) { }   
};

ScriptBuildSetup::ScriptBuildSetup(const BuildSetup& setup): setup(setup)
{
    ScriptObjectFunctionBinding(config, 0);
    ScriptObjectFunctionBinding(target, 0);
}

duk_ret_t ScriptBuildSetup::config(duk_context* context)
{
    duk_push_string(context, configurationToString(setup.configuration));
    return 1;
}

duk_ret_t ScriptBuildSetup::target(duk_context* context)
{
    duk_push_string(context, setup.target->identifier());
    return 1;
}

ScriptModuleBase::ScriptModuleBase(ModuleManifest* moduleManifest): moduleManifest(moduleManifest)
{
    ScriptObjectFunctionBinding(dir, 0);
    ScriptObjectFunctionBinding(source, 0);
    ScriptObjectFunctionBinding(addDependency, 0);
    ScriptObjectFunctionBinding(setType, 0);
    ScriptObjectFunctionBinding(setSolutionLocation, 0);
    ScriptObjectFunctionBinding(link, 1);
}

duk_ret_t ScriptModuleBase::dir(duk_context* context)
{
    duk_push_string(context, moduleManifest->manifestDirectory());
    return 1;
}

duk_ret_t ScriptModuleBase::source(duk_context* context)
{
    duk_push_array(context);

    unsigned int fileCount = moduleManifest->sourceFiles();
    for (unsigned int fileIndex = 0; fileIndex < fileCount; fileIndex++)
    {
        duk_push_string(context, moduleManifest->sourceFile(fileIndex));
        duk_put_prop_index(context, -2, fileIndex);
    }

    return 1;
}

duk_ret_t ScriptModuleBase::addDependency(duk_context* context)
{
    std::string dependency = duk_safe_to_string(context, 0);
    moduleManifest->addDependency(dependency.c_str());
    return 0;
}

duk_ret_t ScriptModuleBase::setType(duk_context* context)
{
    std::string type = duk_safe_to_string(context, 0);
    if (type == "library")
    {
        moduleManifest->setModuleType(MT_Library);
    }
    else if (type == "executable")
    {
        moduleManifest->setModuleType(MT_Executable);
    }
    else
    {
        Utility::PrintLine("Unknown module type: " + type);
        return -1;
    }
    
    return 0;
}

duk_ret_t ScriptModuleBase::setSolutionLocation(duk_context* context)
{
    std::string path = duk_safe_to_string(context, 0);
    moduleManifest->setSolutionLocation(path.c_str());
    return 0;
}

duk_ret_t ScriptModuleBase::link(duk_context* context)
{
    std::string library = duk_safe_to_string(context, 0);
    moduleManifest->addLinkerLibrary(library.c_str());
    return 0;
}

ScriptModuleConfig::ScriptModuleConfig(ModuleManifest* moduleManifest): ScriptModuleBase(moduleManifest)
{
    ScriptObjectFunctionBinding(setName, 1);
    ScriptObjectFunctionBinding(addSource, 1);
    ScriptObjectFunctionBinding(scanSource, 1);
    ScriptObjectFunctionBinding(addIncludePrivate, 1);
    ScriptObjectFunctionBinding(addIncludePublic, 1);
}

duk_ret_t ScriptModuleConfig::setName(duk_context* context)
{
    std::string name = duk_safe_to_string(context, 0);
    moduleManifest->setName(name.c_str());
    return 0;
}

duk_ret_t ScriptModuleConfig::addSource(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    moduleManifest->addSource(pathString.c_str());

    return 0;
}

duk_ret_t ScriptModuleConfig::addIncludePrivate(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    moduleManifest->addIncludePath(AD_Private, pathString.c_str());

    return 0;
}

duk_ret_t ScriptModuleConfig::addIncludePublic(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    moduleManifest->addIncludePath(AD_Public, pathString.c_str());

    return 0;
}

duk_ret_t ScriptModuleConfig::scanSource(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    std::string pattern = std::filesystem::path("**\\").make_preferred().string();
    std::vector<std::string> directories = { pathString };

    while (true)
    {
        size_t index = pathString.find(pattern);
        if (index == std::string::npos) break;

        pathString[index] = '.';
        pathString.erase(index + 1, 1);

        directories.push_back(pathString);
    }

    for (std::string& directory : directories)
    {
        moduleManifest->scanSource(directory.c_str());
    }

    return 0;
}

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
