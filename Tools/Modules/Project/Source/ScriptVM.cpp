#include "ScriptVM.hpp"
#include "Utility.hpp"
#include <fstream>

duk_ret_t ecma_print(duk_context* ctx)
{
	if (duk_is_string(ctx, 0) || duk_is_number(ctx, 0) || duk_is_boolean(ctx, 0)) Utility::PrintLine("> " + std::string(duk_to_string(ctx, 0)));
	return 0;
}

bool ScriptVM::init(std::filesystem::path file)
{
	context = duk_create_heap_default();

	duk_push_c_function(context, ecma_print, 1);
	duk_put_global_string(context, "print");

	return loadFromFile(file);
}

void ScriptVM::destroy()
{
	duk_destroy_heap(context);
	context = nullptr;
}

bool ScriptVM::loadFromFile(const std::filesystem::path& file)
{
	std::ifstream stream(file);
	std::string string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();

	return loadFromString(string);
}

bool ScriptVM::loadFromString(const std::string& string)
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

bool ModuleScript::configure(const BuildSetup&)
{
	duk_push_global_object(context);

	if (duk_get_global_string(context, "configure") == 0)
	{
		Utility::PrintLine("Could not find function 'configure'");
		return false;
	}

	if (duk_pcall(context, 0) != DUK_EXEC_SUCCESS)
	{
		Utility::PrintLine("> " + std::string(duk_safe_to_string(context, -1)));
		return false;
	}

	return true;
}
