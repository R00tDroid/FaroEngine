#pragma once
#include "ManifestInterface.hpp"

class FaroProjectsExports ProjectManifest : public IManifest
{
public:
    static const char* projectManifestExtension();

    ProjectManifest(const char* manifestLocation);
    ~ProjectManifest() override;

    const char* projectName() const;
    const char* uuid() const;

    unsigned int modules() const;
    const char* modulePath(unsigned int index) const;

    bool load();
    bool loadCache();
    bool loadManifest();

private:
    struct Impl;
    Impl* impl = nullptr;
};
