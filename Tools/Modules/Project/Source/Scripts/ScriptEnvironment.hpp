#pragma once
#include <filesystem>
#include <duktape.h>
#include <map>

class ModuleManifest;
struct BuildSetup;

class ScriptEnvironment
{
public:
    bool init(std::filesystem::path file);
    void destroy();

    static duk_ret_t print(duk_context* context);
    static void dumpStack(duk_context* context);

protected:
    duk_context* context = nullptr;
    virtual bool bindEnvironment();

private:
    bool loadFromFile(const std::filesystem::path& file);
    bool loadFromString(const std::string& string);
};

class ScriptObject
{
public:
    void bind(duk_context* context) const;
    std::string print() const;

protected:
    struct ScriptFunction { duk_c_function function = nullptr; duk_int_t args = 0; };
    std::map<const char*, ScriptFunction> functions;
};

#define ScriptObjectFunction(type, function) static duk_ret_t function##_relay(duk_context* context) { duk_push_this(context); duk_get_prop_string(context, -1, "ptr"); void* ptr = duk_get_pointer(context, -1); type* instance = (type*)ptr; if(instance == nullptr){ return DUK_RET_ERROR; } return instance->function(context); } duk_ret_t function(duk_context* context)
#define ScriptObjectFunctionBinding(function, args) functions.insert(std::pair<const char*, ScriptFunction>(#function, { &function##_relay, args }))
