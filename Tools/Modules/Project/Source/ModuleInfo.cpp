#include "ModuleInfo.hpp"
#include <Utility.hpp>
#include <fstream>
#include <sstream>

struct ModuleManifest::Impl
{
    std::string name;
    std::string solutionDirectory;

    std::vector<std::string> linkingLibraries;
    std::vector<std::string> publicDefines;
    std::vector<std::string> privateDefines;

    ModuleType type = MT_Library;

    std::vector<std::string> moduleDependencies;

    std::vector<std::string> privateIncludes;
    std::vector<std::string> publicIncludes;

    std::vector<std::string> sourceFiles;

    std::vector<FolderMount> folderMounts;

    std::string uuid = "";
};

const char* ModuleManifest::moduleManifestExtension()
{
    return ".faromod.js";
}

ModuleManifest::ModuleManifest(const char* manifestLocation) : IManifest(manifestLocation)
{
    impl = new Impl();
}

ModuleManifest::~ModuleManifest()
{
    delete impl;
}

const char* ModuleManifest::name() const
{
    return impl->name.c_str();
}

const char* ModuleManifest::solutionLocation() const
{
    return impl->solutionDirectory.c_str();
}

ProjectManifest* ModuleManifest::project() const
{
    return nullptr; //TODO Return project
}

unsigned int ModuleManifest::defines(AccessDomain type) const
{
    switch (type) {
    case AD_Private:
        return static_cast<unsigned int>(impl->privateDefines.size());
    case AD_Public:
        return static_cast<unsigned int>(impl->publicDefines.size());
    }
    return 0;
}

const char* ModuleManifest::define(AccessDomain type, unsigned int index) const
{
    switch (type) {
    case AD_Private:
        return impl->privateDefines[index].c_str();
    case AD_Public:
        return impl->publicDefines[index].c_str();
    }
    return nullptr;
}

unsigned int ModuleManifest::linkerLibraries() const
{
    return static_cast<unsigned int>(impl->linkingLibraries.size());
}

const char* ModuleManifest::linkerLibrary(unsigned int index) const
{
    return impl->linkingLibraries[index].c_str();
}

ModuleType ModuleManifest::moduleType() const
{
    return impl->type;
}

unsigned int ModuleManifest::dependencyPaths() const
{
    return static_cast<unsigned int>(impl->moduleDependencies.size());
}

const char* ModuleManifest::dependencyPath(unsigned int index) const
{
    return impl->moduleDependencies[index].c_str();
}

unsigned int ModuleManifest::includePaths(AccessDomain type) const
{
    switch (type) {
    case AD_Private:
        return static_cast<unsigned int>(impl->privateIncludes.size());
    case AD_Public:
        return static_cast<unsigned int>(impl->publicIncludes.size());
    }
    return 0;
}

const char* ModuleManifest::includePath(AccessDomain type, unsigned int index) const
{
    switch (type) {
    case AD_Private:
        return impl->privateIncludes[index].c_str();
    case AD_Public:
        return impl->publicIncludes[index].c_str();
    }
    return nullptr;
}

unsigned int ModuleManifest::sourceFiles() const
{
    return static_cast<unsigned int>(impl->sourceFiles.size());
}

const char* ModuleManifest::sourceFile(unsigned int index) const
{
    return impl->sourceFiles[index].c_str();
}

unsigned int ModuleManifest::mounts() const
{
    return static_cast<unsigned int>(impl->folderMounts.size());
}

const FolderMount& ModuleManifest::mount(unsigned int index) const
{
    return impl->folderMounts[index];
}

const char* ModuleManifest::uuid() const
{
    return impl->uuid.c_str();
}

bool ModuleManifest::configure(const Configuration*) const
{
    return false; //TODO Implement
}

bool ModuleManifest::load(const Configuration*) const
{
    return false; //TODO Implement
}

bool ModuleManifest::prebuild(const Configuration*) const
{
    return false; //TODO Implement
}

bool ModuleManifest::postbuild(const Configuration*) const
{
    return false; //TODO Implement
}


bool ModuleManifest::loadCache(const Configuration*)
{
    ModuleManifest* manifest = new ModuleManifest(path);
    manifest->project = project;

    std::string line;

    std::ifstream filesList(manifest->infoDirectory / "Source.txt");
    while (std::getline(filesList, line))
    {
        manifest->sourceFiles.push_back(line);
    }
    filesList.close();

    std::ifstream publicIncludeList(manifest->infoDirectory / "PublicIncludes.txt");
    while (std::getline(publicIncludeList, line))
    {
        manifest->publicIncludes.push_back(line);
    }
    publicIncludeList.close();

    std::ifstream privateIncludeList(manifest->infoDirectory / "PrivateIncludes.txt");
    while (std::getline(privateIncludeList, line))
    {
        manifest->privateIncludes.push_back(line);
    }
    privateIncludeList.close();

    std::ifstream publicDefineList(manifest->infoDirectory / "PublicDefines.txt");
    while (std::getline(publicDefineList, line))
    {
        manifest->publicDefines.push_back(line);
    }
    publicDefineList.close();

    std::ifstream privateDefineList(manifest->infoDirectory / "PrivateDefines.txt");
    while (std::getline(privateDefineList, line))
    {
        manifest->privateDefines.push_back(line);
    }
    privateDefineList.close();

    std::ifstream dependencyList(manifest->infoDirectory / "Dependencies.txt");
    while (std::getline(dependencyList, line))
    {
        manifest->moduleDependencies.push_back(line);
    }
    dependencyList.close();

    std::ifstream libraryList(manifest->infoDirectory / "Libraries.txt");
    while (std::getline(libraryList, line))
    {
        manifest->linkingLibraries.push_back(line);
    }
    libraryList.close();

    std::ifstream moduleType(manifest->infoDirectory / "ModuleType.txt");
    std::stringstream moduleTypeStream;
    moduleTypeStream << moduleType.rdbuf();
    moduleType.close();
    std::string moduleTypeString = moduleTypeStream.str();

    if (moduleTypeString == "lib")
    {
        manifest->type = MT_Library;
    }
    else if (moduleTypeString == "app")
    {
        manifest->type = MT_Executable;
    }

    manifest->uuid = Utility::GetCachedUUID(manifest->infoDirectory / "ModuleId.txt");

    std::ifstream platformFilterList(manifest->infoDirectory / "PlatformFilter.txt");
    std::stringstream platformFilterStream;
    platformFilterStream << platformFilterList.rdbuf();
    platformFilterList.close();
    manifest->platformFilter = platformFilterStream.str();
    platformFilterList.close();

    std::ifstream mountList(manifest->infoDirectory / "Mounts.txt");
    while (true)
    {
        std::string location, mountPoint;
        if (!std::getline(mountList, location)) break;
        if (!std::getline(mountList, mountPoint)) break;

        manifest->folderMounts.push_back({location, mountPoint});
    }
    mountList.close();

    return manifest;
}

bool ModuleManifest::saveCache(const Configuration*) const
{
    std::ofstream filesList(infoDirectory / "Source.txt");
    for (std::string& path : sourceFiles)
    {
        filesList << path.string() << "\n";
    }
    filesList.close();

    std::ofstream publicIncludeList(infoDirectory / "PublicIncludes.txt");
    for (std::string& path : publicIncludes)
    {
        publicIncludeList << path.string() << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(infoDirectory / "PrivateIncludes.txt");
    for (std::string& path : privateIncludes)
    {
        privateIncludeList << path.string() << "\n";
    }
    privateIncludeList.close();

    std::ofstream publicDefineList(infoDirectory / "PublicDefines.txt");
    for (std::string& path : publicDefines)
    {
        publicDefineList << path << "\n";
    }
    publicDefineList.close();

    std::ofstream privateDefinesList(infoDirectory / "PrivateDefines.txt");
    for (std::string& path : privateDefines)
    {
        privateDefinesList << path << "\n";
    }
    privateDefinesList.close();

    std::ofstream dependencyList(infoDirectory / "Dependencies.txt");
    for (std::string dependency : moduleDependencies)
    {
        dependencyList << dependency.string() << "\n";
    }
    dependencyList.close();

    std::ofstream libraryList(infoDirectory / "Libraries.txt");
    for (std::string& path : linkingLibraries)
    {
        libraryList << path.c_str() << "\n";
    }
    libraryList.close();

    std::ofstream moduleType(infoDirectory / "ModuleType.txt");
    switch (type)
    {
        case MT_Library: { moduleType << "lib"; break; }
        case MT_Executable: { moduleType << "app"; break; }
    }
    moduleType.close();

    std::ofstream platformFilterList(infoDirectory / "PlatformFilter.txt");
    platformFilterList << platformFilter;
    platformFilterList.close();

    std::ofstream mountList(infoDirectory / "Mounts.txt");
    for (FolderMount& mount : folderMounts)
    {
        mountList << mount.location.string().c_str() << "\n" << mount.mountPoint.string().c_str() << "\n";
    }
    mountList.close();
}
