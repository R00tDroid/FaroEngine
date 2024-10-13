#pragma once
#include "ManifestInterface.hpp"

class FaroProjectsExports ProjectManifest : public IManifest
{
public:
    static const char* projectManifestExtension();

    ProjectManifest(const char* manifestLocation);
    ~ProjectManifest() override;

    // Name of the project
    const char* projectName() const;

    // Unique identifier for the project
    const char* uuid() const;

    // Amount of modules in the project
    unsigned int modules() const;

    // Path to the module manifest at the specified index.
    const char* modulePath(unsigned int index) const;

    bool load();
    bool loadCache();
    bool loadManifest();

    struct Impl;
private:
    Impl* impl = nullptr;
};
