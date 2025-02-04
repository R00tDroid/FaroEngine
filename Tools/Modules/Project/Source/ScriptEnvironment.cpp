#include "ScriptEnvironment.hpp"
#include "Utility.hpp"
#include <fstream>
#include <map>
#include "ModuleInfo.hpp"

#define EcmaObjectFunction(type, function) static duk_ret_t function##_relay(duk_context* context) { duk_push_this(context); duk_get_prop_string(context, -1, "ptr"); void* ptr = duk_get_pointer(context, -1); type* instance = (type*)ptr; if(instance == nullptr){ return DUK_RET_ERROR; } return instance->function(context); } duk_ret_t function(duk_context* context)
#define EcmaObjectFunctionBinding(function, args) functions.insert(std::pair<const char*, EcmaFunction>(#function, { &function##_relay, args }))

struct EcmaFunction
{
    duk_c_function function = nullptr;
    duk_int_t args = 0;
};

class EcmaObject
{
public:
    void bind(duk_context* context)
    {
        duk_push_object(context);

        duk_push_pointer(context, this);
        duk_put_prop_string(context, -2, "ptr");

        for (const auto& function : functions)
        {
            duk_push_c_function(context, function.second.function, function.second.args);
            duk_put_prop_string(context, -2, function.first);
        }
    }

protected:
    std::map<const char*, EcmaFunction> functions;
};

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

bool ScriptEnvironment::init(std::filesystem::path file)
{
    context = duk_create_heap_default();

    if (!loadFromFile(file)) return false;

    if (!bindEnvironment()) return false;

    return true;
}

bool ScriptEnvironment::bindEnvironment()
{
    duk_push_c_function(context, &ScriptEnvironment::print, 1);
    duk_put_global_string(context, "print");

    return true;
}

void ScriptEnvironment::destroy()
{
    duk_destroy_heap(context);
    context = nullptr;
}

duk_ret_t ScriptEnvironment::print(duk_context* context)
{
    if (duk_is_string(context, 0) || duk_is_number(context, 0) || duk_is_boolean(context, 0))
        Utility::PrintLine("> " + std::string(duk_to_string(context, 0)));
    else
        Utility::PrintLine("print()");
    return 0;
}

void ScriptEnvironment::dumpStack(duk_context* context)
{
    duk_push_context_dump(context);
    Utility::PrintLineD(std::string(duk_to_string(context, -1)));
    duk_pop(context);
}

bool ScriptEnvironment::loadFromFile(const std::filesystem::path& file)
{
    std::ifstream stream(file);
    std::string string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    stream.close();

    return loadFromString(string);
}

bool ScriptEnvironment::loadFromString(const std::string& string)
{
    duk_push_lstring(context, string.c_str(), string.length());

    if (duk_peval(context) != 0)
    {
        Utility::PrintLine("> " + std::string(duk_safe_to_string(context, -1)));
        destroy();
        return false;
    }

    duk_pop(context);

    return true;
}

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
