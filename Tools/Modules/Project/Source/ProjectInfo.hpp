#pragma once
#include <string>
#include <vector>
#include "ManifestInterface.hpp"
#include "Utility.hpp"

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

    bool findModules(const std::vector<std::string>& names, std::vector<ModuleManifest*>& modules) const
    {
        for (const std::string& name : names)
        {
            ModuleManifest* module = findModule(name.c_str());
            if (module == nullptr)
            {
                Utility::PrintLine("Failed to find module " + name);
                return false;
            }

            modules.push_back(module);
        }
        return true;
    }

    bool configure();
    bool load(const BuildSetup& setup);

    struct Impl;
private:
    Impl* impl = nullptr;
};
