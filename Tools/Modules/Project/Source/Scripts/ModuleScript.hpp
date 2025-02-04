#pragma once
#include "ScriptEnvironment.hpp"

class ModuleManifest;
struct BuildSetup;


class ModuleScript : public ScriptEnvironment
{
public:
    bool configure(const BuildSetup&, ModuleManifest* module);

protected:
    bool bindEnvironment() override;

    static duk_ret_t config(duk_context* context);
    static duk_ret_t target(duk_context* context);
};
