#pragma once
#include "BuildSetup.hpp"
#include "FaroProjectsExports.generated.h"
#include "ManifestInterface.hpp"

class ProjectManifest;

enum ModuleType
{
    MT_Library,
    MT_Executable
};

enum AccessDomain 
{
    AD_Private,
    AD_Public
};

class FaroProjectsExports FolderMount
{
public:
    FolderMount(const char* location, const char* mountPoint);
    ~FolderMount();

    const char* location() const;
    const char* mountPoint() const;

private:
    struct Impl;
    Impl* impl = nullptr;
};

class FaroProjectsExports ModuleManifest : public IManifest
{
public:
    static const char* moduleManifestExtension();

    ModuleManifest(const char* manifestLocation);
    ~ModuleManifest() override;

    // Name of the module
    const char* name() const;

    // Location within the solution hierarchy
    const char* solutionLocation() const;

    // Project this module belongs to
    ProjectManifest* project() const;

    unsigned int defines(AccessDomain type) const;
    const char* define(AccessDomain type, unsigned int index) const;

    unsigned int linkerLibraries() const;
    const char* linkerLibrary(unsigned int index) const;

    ModuleType moduleType() const;

    // List of modules this module depends on
    unsigned int dependencies() const;
    ModuleManifest* dependency(unsigned int index) const;

    unsigned int includePaths(AccessDomain type) const;
    const char* includePath(AccessDomain type, unsigned int index) const;

    unsigned int sourceFiles() const;
    const char* sourceFile(unsigned int index) const;

    unsigned int mounts() const;
    const FolderMount& mount(unsigned int index) const;

    const char* uuid() const override;

    bool configure(const BuildSetup* setup) const;
    bool load(const BuildSetup* setup) const;

    bool prebuild(const BuildSetup* setup) const;
    bool postbuild(const BuildSetup* setup) const;

private:
    struct Impl;
    Impl* impl = nullptr;

    bool saveCache(const BuildSetup* setup) const;
    bool loadCache(const BuildSetup* setup);
};
