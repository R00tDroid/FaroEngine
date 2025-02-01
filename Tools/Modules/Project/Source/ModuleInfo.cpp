#include "ModuleInfo.hpp"
#include <Utility.hpp>
#include <fstream>
#include <sstream>
#include "ScriptVM.hpp"

struct FolderMount::Impl
{
    std::string location;
    std::string mountPoint;
};

struct ModuleManifest::Impl
{
    std::string name;
    std::string solutionDirectory;

    std::vector<std::string> linkingLibraries;
    std::vector<std::string> publicDefines;
    std::vector<std::string> privateDefines;

    ModuleType type = MT_Library;

    std::vector<std::string> moduleDependencyPaths; //TODO Resolve path to module
    std::vector<ModuleManifest*> moduleDependencies;

    std::vector<std::string> privateIncludes;
    std::vector<std::string> publicIncludes;

    std::vector<std::string> sourceFiles;

    std::vector<FolderMount> folderMounts;

    std::string uuid = "";
};

FolderMount::FolderMount(const char* location, const char* mountPoint)
{
    impl = new Impl();
    impl->location = location;
    impl->mountPoint = mountPoint;
}

FolderMount::~FolderMount()
{
    delete impl;
}

const char* FolderMount::location() const
{
    return impl->location.c_str();
}

const char* FolderMount::mountPoint() const
{
    return impl->mountPoint.c_str();
}

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

unsigned int ModuleManifest::dependencies() const
{
    return static_cast<unsigned int>(impl->moduleDependencies.size());
}

ModuleManifest* ModuleManifest::dependency(unsigned int index) const
{
    return impl->moduleDependencies[index];
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

bool ModuleManifest::configure(const BuildSetup& setup) const
{
    ModuleScript vm;
    if (!vm.init(manifestPath()))
    {
        return false;
    }

    if (!vm.configure(setup))
    {
        return false;
    }

    vm.destroy();

    return true;
}

bool ModuleManifest::load(const BuildSetup&) const
{
    return false; //TODO Implement
}

bool ModuleManifest::prebuild(const BuildSetup&) const
{
    return false; //TODO Implement
}

bool ModuleManifest::postbuild(const BuildSetup&) const
{
    return false; //TODO Implement
}

std::filesystem::path getBuildSetupPath(const ModuleManifest* module, const BuildSetup& setup)
{
    std::filesystem::path cacheFolder = module->cacheDirectory();
    std::filesystem::path configsFolder = cacheFolder / "Config";
    std::string configName(setup.identifier());
    return configsFolder / configName;
}

bool ModuleManifest::loadCache(const BuildSetup& config)
{
    std::filesystem::path cacheFolder = getBuildSetupPath(this, config);

    std::string line;

    std::ifstream filesList(cacheFolder / "Source.txt");
    while (std::getline(filesList, line))
    {
        impl->sourceFiles.push_back(line);
    }
    filesList.close();

    std::ifstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    while (std::getline(publicIncludeList, line))
    {
        impl->publicIncludes.push_back(line);
    }
    publicIncludeList.close();

    std::ifstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    while (std::getline(privateIncludeList, line))
    {
        impl->privateIncludes.push_back(line);
    }
    privateIncludeList.close();

    std::ifstream publicDefineList(cacheFolder / "PublicDefines.txt");
    while (std::getline(publicDefineList, line))
    {
        impl->publicDefines.push_back(line);
    }
    publicDefineList.close();

    std::ifstream privateDefineList(cacheFolder / "PrivateDefines.txt");
    while (std::getline(privateDefineList, line))
    {
        impl->privateDefines.push_back(line);
    }
    privateDefineList.close();

    std::ifstream dependencyList(cacheFolder / "Dependencies.txt");
    while (std::getline(dependencyList, line))
    {
        impl->moduleDependencyPaths.push_back(line);
    }
    dependencyList.close();

    std::ifstream libraryList(cacheFolder / "Libraries.txt");
    while (std::getline(libraryList, line))
    {
        impl->linkingLibraries.push_back(line);
    }
    libraryList.close();

    std::ifstream moduleType(cacheFolder / "ModuleType.txt");
    std::stringstream moduleTypeStream;
    moduleTypeStream << moduleType.rdbuf();
    moduleType.close();
    std::string moduleTypeString = moduleTypeStream.str();

    if (moduleTypeString == "lib")
    {
        impl->type = MT_Library;
    }
    else if (moduleTypeString == "app")
    {
        impl->type = MT_Executable;
    }

    char uuid[UUID_LENGTH];
    std::filesystem::path uuidStorage = cacheFolder / "ModuleId.txt";
    if (Utility::GetCachedUUID(uuidStorage.string().c_str(), uuid))
    {
        impl->uuid = uuid;
    }

    std::ifstream mountList(cacheFolder / "Mounts.txt");
    while (true)
    {
        std::string location, mountPoint;
        if (!std::getline(mountList, location)) break;
        if (!std::getline(mountList, mountPoint)) break;

        impl->folderMounts.push_back({location.c_str(), mountPoint.c_str()});
    }
    mountList.close();

    return true;
}

bool ModuleManifest::saveCache(const BuildSetup& config) const
{
    std::filesystem::path cacheFolder = getBuildSetupPath(this, config);

    std::ofstream filesList(cacheFolder / "Source.txt");
    for (std::string& path : impl->sourceFiles)
    {
        filesList << path << "\n";
    }
    filesList.close();

    std::ofstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    for (std::string& path : impl->publicIncludes)
    {
        publicIncludeList << path << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    for (std::string& path : impl->privateIncludes)
    {
        privateIncludeList << path << "\n";
    }
    privateIncludeList.close();

    std::ofstream publicDefineList(cacheFolder / "PublicDefines.txt");
    for (std::string& path : impl->publicDefines)
    {
        publicDefineList << path << "\n";
    }
    publicDefineList.close();

    std::ofstream privateDefinesList(cacheFolder / "PrivateDefines.txt");
    for (std::string& path : impl->privateDefines)
    {
        privateDefinesList << path << "\n";
    }
    privateDefinesList.close();

    std::ofstream dependencyList(cacheFolder / "Dependencies.txt");
    for (ModuleManifest* dependency : impl->moduleDependencies)
    {
        dependencyList << dependency->manifestPath() << "\n";
    }
    dependencyList.close();

    std::ofstream libraryList(cacheFolder / "Libraries.txt");
    for (std::string& path : impl->linkingLibraries)
    {
        libraryList << path.c_str() << "\n";
    }
    libraryList.close();

    std::ofstream moduleType(cacheFolder / "ModuleType.txt");
    switch (impl->type)
    {
        case MT_Library: { moduleType << "lib"; break; }
        case MT_Executable: { moduleType << "app"; break; }
    }
    moduleType.close();

    std::ofstream mountList(cacheFolder / "Mounts.txt");
    for (FolderMount& mount : impl->folderMounts)
    {
        mountList << mount.location() << "\n" << mount.mountPoint() << "\n";
    }
    mountList.close();

    return true;
}
