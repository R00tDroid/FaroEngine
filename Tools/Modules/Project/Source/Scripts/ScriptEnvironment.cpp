#include "ScriptEnvironment.hpp"
#include "Utility.hpp"
#include <fstream>

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
