#pragma once
#include <filesystem>
#include <duktape.h>

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

class ModuleScript : public ScriptEnvironment
{
public:
    bool configure(const BuildSetup&);

protected:
    bool bindEnvironment() override;

    static duk_ret_t config(duk_context* context);
    static duk_ret_t target(duk_context* context);
};
