#include "ModuleScript.hpp"
#include "ModuleInfo.hpp"
#include "Utility.hpp"
#include <glob/glob.hpp>

ScriptBuildSetup::ScriptBuildSetup(const BuildSetup& setup): setup(setup)
{
    ScriptObjectFunctionBinding(getConfig, 0);
    ScriptObjectFunctionBinding(getTarget, 0);
}

duk_ret_t ScriptBuildSetup::getConfig(duk_context* context)
{
    duk_push_string(context, configurationToString(setup.configuration));
    return 1;
}

duk_ret_t ScriptBuildSetup::getTarget(duk_context* context)
{
    duk_push_string(context, setup.target->identifier());
    return 1;
}

ScriptModuleBase::ScriptModuleBase(const ModuleManifest* moduleManifest): moduleManifest(moduleManifest)
{
    ScriptObjectFunctionBinding(getDirectory, 0);
    ScriptObjectFunctionBinding(addLinkerLibrary, 1);
    ScriptObjectFunctionBinding(addIncludePrivate, 1);
    ScriptObjectFunctionBinding(addIncludePublic, 1);
}

duk_ret_t ScriptModuleBase::getDirectory(duk_context* context)
{
    duk_push_string(context, moduleManifest->manifestDirectory());
    return 1;
}

duk_ret_t ScriptModuleConfig::addDependency(duk_context* context)
{
    std::string dependency = duk_safe_to_string(context, 0);
    dependencies.push_back(dependency);
    return 0;
}

duk_ret_t ScriptModuleConfig::setType(duk_context* context)
{
    std::string typeString = duk_safe_to_string(context, 0);
    if (typeString == "library")
    {
        type = MT_Library;
    }
    else if (typeString == "executable")
    {
        type = MT_Executable;
    }
    else
    {
        Utility::PrintLine("Unknown module type: " + typeString);
        return -1;
    }
    
    return 0;
}

duk_ret_t ScriptModuleConfig::setSolutionLocation(duk_context* context)
{
    std::string path = duk_safe_to_string(context, 0);
    solutionPath = path;
    return 0;
}

duk_ret_t ScriptModuleBase::addLinkerLibrary(duk_context* context)
{
    std::string library = duk_safe_to_string(context, 0);
    linkerLibraries.push_back(library);
    return 0;
}

ScriptModuleConfig::ScriptModuleConfig(const ModuleManifest* moduleManifest): ScriptModuleBase(moduleManifest)
{
    ScriptObjectFunctionBinding(setName, 1);
    ScriptObjectFunctionBinding(addSource, 1);
    ScriptObjectFunctionBinding(scanSource, 1);
    ScriptObjectFunctionBinding(setType, 1);
    ScriptObjectFunctionBinding(addDependency, 1);
    ScriptObjectFunctionBinding(setSolutionLocation, 1);
}

duk_ret_t ScriptModuleConfig::setName(duk_context* context)
{
    name = duk_safe_to_string(context, 0);
    return 0;
}

duk_ret_t ScriptModuleConfig::addSource(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    sourceFiles.push_back(pathString);

    return 0;
}

duk_ret_t ScriptModuleBase::addIncludePrivate(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    includesPrivate.push_back(pathString);

    return 0;
}

duk_ret_t ScriptModuleBase::addIncludePublic(duk_context* context)
{
    std::filesystem::path path(duk_safe_to_string(context, 0));
    path.make_preferred();
    path = weakly_canonical(path);
    std::string pathString = path.string();

    includesPublic.push_back(pathString);

    return 0;
}

ScriptModuleSetup::ScriptModuleSetup(const ModuleManifest* moduleManifest): ScriptModuleBase(moduleManifest) {}

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
        for (auto file : glob::rglob(directory))
        {
            file = std::filesystem::weakly_canonical(file);
            std::string fileString = file.string();
            sourceFiles.push_back(fileString);
        }
    }

    return 0;
}

bool ModuleScript::configureModule(ScriptModuleConfig& buildModule)
{
    duk_push_global_object(context);

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

bool ModuleScript::configureSetup(ScriptBuildSetup& buildSetup, ScriptModuleSetup& buildModule)
{
    duk_push_global_object(context);

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
