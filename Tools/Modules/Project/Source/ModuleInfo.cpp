#include "ModuleInfo.hpp"
#include <Utility.hpp>
#include <fstream>
#include <sstream>
#include "Scripts/ModuleScript.hpp"
#include <glob/glob.hpp>

struct FolderMount::Impl
{
    std::string location;
    std::string mountPoint;
};

#define CombineImpls(name) std::vector<std::string> _##name; const std::vector<std::string>& name() { size_t size = module.name.size() + setup.name.size(); if (_##name.size() != size) { _##name.clear(); _##name.reserve(size); _##name.insert(_##name.end(), module.name.begin(), module.name.end()); _##name.insert(_##name.end(), setup.name.begin(), setup.name.end()); } return _##name; }

struct ModuleManifest::Impl
{
    struct ModuleImpl
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
    ModuleImpl module;

    struct SetupImpl
    {
        std::vector<std::string> linkingLibraries;
        std::vector<std::string> publicDefines;
        std::vector<std::string> privateDefines;

        std::vector<std::string> privateIncludes;
        std::vector<std::string> publicIncludes;
    };
    SetupImpl setup;

    CombineImpls(linkingLibraries);
    CombineImpls(publicDefines);
    CombineImpls(privateDefines);

    CombineImpls(privateIncludes);
    CombineImpls(publicIncludes);
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
    return impl->module.name.c_str();
}

void ModuleManifest::setName(const char* name)
{
    impl->module.name = name;
}

const char* ModuleManifest::solutionLocation() const
{
    return impl->module.solutionDirectory.c_str();
}

void ModuleManifest::setSolutionLocation(const char* location)
{
    impl->module.solutionDirectory = location;
}

void ModuleManifest::addLinkerLibrary(const char* library) const
{
    impl->module.linkingLibraries.push_back(library);
}

void ModuleManifest::setModuleType(ModuleType type) const
{
    impl->module.type = type;
}

void ModuleManifest::addDependency(const char* dependency)
{
    impl->module.moduleDependencyPaths.push_back(dependency);
}

void ModuleManifest::addIncludePath(AccessDomain type, const char* path) const
{
    switch (type)
    {
        case AD_Private:
        {
            impl->module.privateIncludes.push_back(path);
            break;
        }
        case AD_Public:
        {
            impl->module.publicIncludes.push_back(path);
            break;
        }
    }
}

void ModuleManifest::addSource(const char* path) const
{
    impl->module.sourceFiles.push_back(path);
}

ProjectManifest* ModuleManifest::project() const
{
    return nullptr; //TODO Return project
}

unsigned int ModuleManifest::defines(AccessDomain type) const
{
    switch (type) {
    case AD_Private:
        return static_cast<unsigned int>(impl->privateDefines().size());
    case AD_Public:
        return static_cast<unsigned int>(impl->publicDefines().size());
    }
    return 0;
}

const char* ModuleManifest::define(AccessDomain type, unsigned int index) const
{
    switch (type) {
    case AD_Private:
        return impl->privateDefines()[index].c_str();
    case AD_Public:
        return impl->publicDefines()[index].c_str();
    }
    return nullptr;
}

unsigned int ModuleManifest::linkerLibraries() const
{
    return static_cast<unsigned int>(impl->linkingLibraries().size());
}

const char* ModuleManifest::linkerLibrary(unsigned int index) const
{
    return impl->linkingLibraries()[index].c_str();
}

ModuleType ModuleManifest::moduleType() const
{
    return impl->module.type;
}

unsigned int ModuleManifest::dependencies() const
{
    return static_cast<unsigned int>(impl->module.moduleDependencies.size());
}

ModuleManifest* ModuleManifest::dependency(unsigned int index) const
{
    return impl->module.moduleDependencies[index];
}

unsigned int ModuleManifest::includePaths(AccessDomain type) const
{
    switch (type) {
    case AD_Private:
        return static_cast<unsigned int>(impl->privateIncludes().size());
    case AD_Public:
        return static_cast<unsigned int>(impl->publicIncludes().size());
    }
    return 0;
}

const char* ModuleManifest::includePath(AccessDomain type, unsigned int index) const
{
    switch (type) {
    case AD_Private:
        return impl->privateIncludes()[index].c_str();
    case AD_Public:
        return impl->publicIncludes()[index].c_str();
    }
    return nullptr;
}

unsigned int ModuleManifest::sourceFiles() const
{
    return static_cast<unsigned int>(impl->module.sourceFiles.size());
}

const char* ModuleManifest::sourceFile(unsigned int index) const
{
    return impl->module.sourceFiles[index].c_str();
}

void ModuleManifest::scanSource(const char* filter) const
{
    Utility::PrintLineD("Scanning: " + std::string(filter));
    unsigned int files = 0;
    for (auto file : glob::rglob(filter))
    {
        file = weakly_canonical(file);
        std::string fileString = file.string();
        addSource(fileString.c_str());
        files++;
    }
    Utility::PrintLineD("Found files: " + std::to_string(files));
}

unsigned int ModuleManifest::mounts() const
{
    return static_cast<unsigned int>(impl->module.folderMounts.size());
}

const FolderMount& ModuleManifest::mount(unsigned int index) const
{
    return impl->module.folderMounts[index];
}

const char* ModuleManifest::uuid() const
{
    return impl->module.uuid.c_str();
}

bool ModuleManifest::configure()
{
    ModuleScript vm;
    if (!vm.init(manifestPath()))
    {
        return false;
    }

    if (!vm.configureModule(this))
    {
        return false;
    }

    unsigned int setupCount = buildSetups();
    for (unsigned int setupIndex = 0; setupIndex < setupCount; setupIndex++)
    {
        BuildSetup setup = buildSetup(setupIndex);
        Utility::PrintLineD(std::string("Configuring: ") + setup.identifier());

        if (!vm.configureSetup(setup, this))
        {
            return false;
        }

        //TODO Store config
    }

    return true;
}

bool ModuleManifest::load(const BuildSetup&)
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
        impl->module.sourceFiles.push_back(line);
    }
    filesList.close();

    std::ifstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    while (std::getline(publicIncludeList, line))
    {
        impl->module.publicIncludes.push_back(line);
    }
    publicIncludeList.close();

    std::ifstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    while (std::getline(privateIncludeList, line))
    {
        impl->module.privateIncludes.push_back(line);
    }
    privateIncludeList.close();

    std::ifstream publicDefineList(cacheFolder / "PublicDefines.txt");
    while (std::getline(publicDefineList, line))
    {
        impl->module.publicDefines.push_back(line);
    }
    publicDefineList.close();

    std::ifstream privateDefineList(cacheFolder / "PrivateDefines.txt");
    while (std::getline(privateDefineList, line))
    {
        impl->module.privateDefines.push_back(line);
    }
    privateDefineList.close();

    std::ifstream dependencyList(cacheFolder / "Dependencies.txt");
    while (std::getline(dependencyList, line))
    {
        impl->module.moduleDependencyPaths.push_back(line);
    }
    dependencyList.close();

    std::ifstream libraryList(cacheFolder / "Libraries.txt");
    while (std::getline(libraryList, line))
    {
        impl->module.linkingLibraries.push_back(line);
    }
    libraryList.close();

    std::ifstream moduleType(cacheFolder / "ModuleType.txt");
    std::stringstream moduleTypeStream;
    moduleTypeStream << moduleType.rdbuf();
    moduleType.close();
    std::string moduleTypeString = moduleTypeStream.str();

    if (moduleTypeString == "lib")
    {
        impl->module.type = MT_Library;
    }
    else if (moduleTypeString == "app")
    {
        impl->module.type = MT_Executable;
    }

    char uuid[UUID_LENGTH];
    std::filesystem::path uuidStorage = cacheFolder / "ModuleId.txt";
    if (Utility::GetCachedUUID(uuidStorage.string().c_str(), uuid))
    {
        impl->module.uuid = uuid;
    }

    std::ifstream mountList(cacheFolder / "Mounts.txt");
    while (true)
    {
        std::string location, mountPoint;
        if (!std::getline(mountList, location)) break;
        if (!std::getline(mountList, mountPoint)) break;

        impl->module.folderMounts.push_back({location.c_str(), mountPoint.c_str()});
    }
    mountList.close();

    return true;
}

bool ModuleManifest::saveCache(const BuildSetup& config) const
{
    std::filesystem::path cacheFolder = getBuildSetupPath(this, config);

    std::ofstream filesList(cacheFolder / "Source.txt");
    for (std::string& path : impl->module.sourceFiles)
    {
        filesList << path << "\n";
    }
    filesList.close();

    std::ofstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    for (std::string& path : impl->module.publicIncludes)
    {
        publicIncludeList << path << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    for (std::string& path : impl->module.privateIncludes)
    {
        privateIncludeList << path << "\n";
    }
    privateIncludeList.close();

    std::ofstream publicDefineList(cacheFolder / "PublicDefines.txt");
    for (std::string& path : impl->module.publicDefines)
    {
        publicDefineList << path << "\n";
    }
    publicDefineList.close();

    std::ofstream privateDefinesList(cacheFolder / "PrivateDefines.txt");
    for (std::string& path : impl->module.privateDefines)
    {
        privateDefinesList << path << "\n";
    }
    privateDefinesList.close();

    std::ofstream dependencyList(cacheFolder / "Dependencies.txt");
    for (ModuleManifest* dependency : impl->module.moduleDependencies)
    {
        dependencyList << dependency->manifestPath() << "\n";
    }
    dependencyList.close();

    std::ofstream libraryList(cacheFolder / "Libraries.txt");
    for (std::string& path : impl->module.linkingLibraries)
    {
        libraryList << path.c_str() << "\n";
    }
    libraryList.close();

    std::ofstream moduleType(cacheFolder / "ModuleType.txt");
    switch (impl->module.type)
    {
        case MT_Library: { moduleType << "lib"; break; }
        case MT_Executable: { moduleType << "app"; break; }
    }
    moduleType.close();

    std::ofstream mountList(cacheFolder / "Mounts.txt");
    for (FolderMount& mount : impl->module.folderMounts)
    {
        mountList << mount.location() << "\n" << mount.mountPoint() << "\n";
    }
    mountList.close();

    return true;
}
