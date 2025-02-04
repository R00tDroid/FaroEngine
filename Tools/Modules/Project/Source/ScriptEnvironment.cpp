#include "ScriptEnvironment.hpp"
#include "Utility.hpp"
#include <fstream>

#define EcmaObjectFunction(type, function) static duk_ret_t function##_relay(duk_context* context) { duk_push_this(context); duk_get_prop_string(context, -1, "ptr"); void* ptr = duk_get_pointer(context, -1); type* instance = (type*)ptr; if(instance == nullptr){ return DUK_RET_ERROR; } return instance->function(context); } duk_ret_t function(duk_context* context)
#define EcmaObjectFunctionBinding(function, args) functions.insert(std::pair<const char*, EcmaFunction>(#function, { &function##_relay, args }))

struct EcmaFunction
{
    duk_c_function function = nullptr;
    unsigned int args = 0;
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

duk_ret_t ecma_print(duk_context* ctx)
{
	if (duk_is_string(ctx, 0) || duk_is_number(ctx, 0) || duk_is_boolean(ctx, 0))
		Utility::PrintLine("> " + std::string(duk_to_string(ctx, 0)));
	else
		Utility::PrintLine("print()");
	return 0;
}

bool ScriptEnvironment::init(std::filesystem::path file)
{
	context = duk_create_heap_default();

	if (!loadFromFile(file)) return false;

	if (!bindEnvironment()) return false;

	return true;
}

bool ScriptEnvironment::bindEnvironment()
{
	duk_push_c_function(context, ecma_print, 1);
	duk_put_global_string(context, "print");

	return true;
}

void ScriptEnvironment::printStack()
{
	duk_push_context_dump(context);
	Utility::PrintLineD(std::string(duk_to_string(context, -1)));
	duk_pop(context);
}

void ScriptEnvironment::destroy()
{
	duk_destroy_heap(context);
	context = nullptr;
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

bool ModuleScript::configure(const BuildSetup& setup)
{
	duk_push_global_object(context);

    EcmaBuildSetup buildSetup(setup);

	if (duk_get_global_string(context, "configure") == 0)
	{
		Utility::PrintLine("Could not find function 'configure'");
		return false;
	}


    buildSetup.bind(context);

    printStack();

	if (duk_pcall(context, 1) != DUK_EXEC_SUCCESS)
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
