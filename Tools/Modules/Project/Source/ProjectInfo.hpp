#pragma once
#include "ManifestInterface.hpp"

struct BuildSetup;
class ModuleManifest;

class FaroProjectsExports ProjectManifest : public IManifest
{
public:
    static const char* projectManifestExtension();

    ProjectManifest(const char* manifestLocation);
    ~ProjectManifest() override;

    // Name of the project
    const char* projectName() const;

    const char* uuid() const override;

    // Amount of modules in the project
    unsigned int modules() const;

    // Path to the module manifest at the specified index.
    ModuleManifest* module(unsigned int index) const;

    ModuleManifest* findModule(const char* name) const;

    bool configure();
    bool load(const BuildSetup& setup);

    struct Impl;
private:
    Impl* impl = nullptr;
};
