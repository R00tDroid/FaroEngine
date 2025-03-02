#pragma once
#include <filesystem>
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
    AD_Public,
    AD_ENUMSIZE
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

    std::vector<std::filesystem::path> moduleIncludes() const
    {
        std::vector<std::filesystem::path> includes = treeIncludes();
        for (unsigned int i = 0; i < includePaths(AD_Private); i++)
        {
            includes.push_back(includePath(AD_Private, i));
        }
        return includes;
    }

    unsigned int sourceFiles() const;
    const char* sourceFile(unsigned int index) const;

    unsigned int mounts() const;
    const FolderMount& mount(unsigned int index) const;

    const char* uuid() const override;

    bool configure();
    bool load(const BuildSetup& setup);
    bool resolve(ProjectManifest* project);

    bool prebuild(const BuildSetup& setup) const;
    bool postbuild(const BuildSetup& setup) const;

private:
    std::vector<std::filesystem::path> treeIncludes() const
    {
        std::vector<std::filesystem::path> includes;

        for (unsigned int i = 0; i < dependencies(); i++)
        {
            std::vector<std::filesystem::path> dependencyIncludes = dependency(i)->treeIncludes();
            for (const std::filesystem::path& include : dependencyIncludes)
            {
                includes.push_back(include);
            }
        }

        for (unsigned int i = 0; i < includePaths(AD_Public); i++)
        {
            includes.push_back(includePath(AD_Public, i));
        }
        return includes;
    }

    struct Impl;
    Impl* impl = nullptr;

    bool loadCache(const BuildSetup& setup);
};
