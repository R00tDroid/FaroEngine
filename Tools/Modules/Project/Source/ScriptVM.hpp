#pragma once
#include <filesystem>
#include <duktape.h>

struct BuildSetup;

class ScriptVM
{
public:
    bool init(std::filesystem::path file);
    void destroy();

protected:
    duk_context* context = nullptr;

private:
    bool loadFromFile(std::filesystem::path file);
    bool loadFromString(std::string& string);
};

class ModuleScript : public ScriptVM
{
public:
    bool configure(const BuildSetup&);
};
