#include "ScriptEnvironment.hpp"
#include "Utility.hpp"
#include <fstream>

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

	EcmaBuildSetup(context, setup);

	if (duk_get_global_string(context, "configure") == 0)
	{
		Utility::PrintLine("Could not find function 'configure'");
		return false;
	}

duk_push_object(context);
	//ECMAOBJECT_EXPOSE_FUNCTION(context, -2, EcmaBuildSetup, config, 0)
	//ECMAOBJECT_EXPOSE_FUNCTION(context, -2, EcmaBuildSetup, target, 0)

	duk_push_c_function(context, &ModuleScript::config, 0);
	duk_put_prop_string(context, -2, "config");
	duk_push_c_function(context, &ModuleScript::target, 0);
	duk_put_prop_string(context, -2, "target");

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

duk_ret_t ModuleScript::config(duk_context* context)
{
	duk_push_string(context, "Release");
	return 1;
}

duk_ret_t ModuleScript::target(duk_context* context)
{
	duk_push_string(context, "WindowsX64");
	return 1;
}
