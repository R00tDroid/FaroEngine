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

    bool configureModule(const ModuleManifest* module, ModuleScript& vm);
    bool configureSetup(const ModuleManifest* module, ModuleScript& vm, const BuildSetup& setup);
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

const char* ModuleManifest::solutionLocation() const
{
    return impl->module.solutionDirectory.c_str();
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

    if (!impl->configureModule(this, vm))
    {
        return false;
    }

    unsigned int setupCount = buildSetups();
    for (unsigned int setupIndex = 0; setupIndex < setupCount; setupIndex++)
    {
        BuildSetup setup = buildSetup(setupIndex);
        Utility::PrintLineD(std::string("Configuring: ") + setup.identifier());

        if (!impl->configureSetup(this, vm, setup))
        {
            return false;
        }
    }

    return true;
}

std::filesystem::path getConfigDirectory(const ModuleManifest* module)
{
    std::filesystem::path cacheFolder = module->cacheDirectory();
    std::filesystem::path configsFolder = cacheFolder / "Config";
    return configsFolder;
}

std::filesystem::path getModuleConfigDirectory(const ModuleManifest* module)
{
    std::filesystem::path configsFolder = getConfigDirectory(module) / "Module";
    Utility::EnsureDirectory(configsFolder.string().c_str());
    return configsFolder;
}

std::filesystem::path getSetupConfigDirectory(const ModuleManifest* module, const BuildSetup& setup)
{
    std::string setupName = setup.identifier();
    std::filesystem::path configsFolder = getConfigDirectory(module) / ("Setup_" + setupName);
    Utility::EnsureDirectory(configsFolder.string().c_str());
    return configsFolder;
}

bool ModuleManifest::Impl::configureModule(const ModuleManifest* manifest, ModuleScript& vm)
{
    ScriptModuleConfig buildModule(manifest);
    if (!vm.configureModule(buildModule))
    {
        return false;
    }

    std::filesystem::path cacheFolder = getModuleConfigDirectory(manifest);

    std::ofstream filesList(cacheFolder / "Source.txt");
    for (std::string& path : buildModule.sourceFiles)
    {
        filesList << path << "\n";
    }
    filesList.close();

    std::ofstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    for (std::string& path : buildModule.includesPublic)
    {
        publicIncludeList << path << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    for (std::string& path : buildModule.includesPrivate)
    {
        privateIncludeList << path << "\n";
    }
    privateIncludeList.close();

    std::ofstream publicDefineList(cacheFolder / "PublicDefines.txt");
    for (std::string& path : buildModule.definesPublic)
    {
        publicDefineList << path << "\n";
    }
    publicDefineList.close();

    std::ofstream privateDefinesList(cacheFolder / "PrivateDefines.txt");
    for (std::string& path : buildModule.definesPrivate)
    {
        privateDefinesList << path << "\n";
    }
    privateDefinesList.close();

    std::ofstream dependencyList(cacheFolder / "Dependencies.txt");
    for (std::string& dependency : buildModule.dependencies)
    {
        dependencyList << dependency << "\n";
    }
    dependencyList.close();

    std::ofstream libraryList(cacheFolder / "Libraries.txt");
    for (std::string& path : buildModule.linkerLibraries)
    {
        libraryList << path.c_str() << "\n";
    }
    libraryList.close();

    std::ofstream moduleType(cacheFolder / "ModuleType.txt");
    switch (buildModule.type)
    {
        case MT_Library: { moduleType << "lib"; break; }
        case MT_Executable: { moduleType << "app"; break; }
    }
    moduleType.close();

    return true;
}

bool ModuleManifest::Impl::configureSetup(const ModuleManifest* manifest, ModuleScript& vm, const BuildSetup& configureSetup)
{
    ScriptBuildSetup buildSetup(configureSetup);
    ScriptModuleSetup buildModule(manifest);

    if (!vm.configureSetup(buildSetup, buildModule))
    {
        return false;
    }

    std::filesystem::path cacheFolder = getSetupConfigDirectory(manifest, configureSetup);

    std::ofstream publicIncludeList(cacheFolder / "PublicIncludes.txt");
    for (std::string& path : buildModule.includesPublic)
    {
        publicIncludeList << path << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(cacheFolder / "PrivateIncludes.txt");
    for (std::string& path : buildModule.includesPrivate)
    {
        privateIncludeList << path << "\n";
    }
    privateIncludeList.close();

    std::ofstream publicDefineList(cacheFolder / "PublicDefines.txt");
    for (std::string& path : buildModule.definesPublic)
    {
        publicDefineList << path << "\n";
    }
    publicDefineList.close();

    std::ofstream privateDefinesList(cacheFolder / "PrivateDefines.txt");
    for (std::string& path : buildModule.definesPrivate)
    {
        privateDefinesList << path << "\n";
    }
    privateDefinesList.close();

    std::ofstream libraryList(cacheFolder / "Libraries.txt");
    for (std::string& path : buildModule.linkerLibraries)
    {
        libraryList << path.c_str() << "\n";
    }
    libraryList.close();

    return true;
}

bool ModuleManifest::load(const BuildSetup& buildSetup)
{
    if (!loadCache(buildSetup))
    {
        return false;
    }
    return true;
}

bool ModuleManifest::prebuild(const BuildSetup&) const
{
    return false; //TODO Implement
}

bool ModuleManifest::postbuild(const BuildSetup&) const
{
    return false; //TODO Implement
}

void readLines(std::filesystem::path path, std::vector<std::string>& array)
{
    std::string line;
    std::ifstream filesList(path);
    while (std::getline(filesList, line))
    {
        array.push_back(line);
    }
    filesList.close();
}

bool ModuleManifest::loadCache(const BuildSetup& config)
{
    std::filesystem::path cacheFolderModule = getModuleConfigDirectory(this);
    std::filesystem::path cacheFolderSetup = getSetupConfigDirectory(this, config);

    std::string line;

    readLines(cacheFolderModule / "Source.txt", impl->module.sourceFiles);
    readLines(cacheFolderModule / "PublicIncludes.txt", impl->module.publicIncludes);
    readLines(cacheFolderModule / "PrivateIncludes.txt", impl->module.privateIncludes);
    readLines(cacheFolderModule / "PublicDefines.txt", impl->module.publicDefines);
    readLines(cacheFolderModule / "PrivateDefines.txt", impl->module.privateDefines);
    readLines(cacheFolderModule / "Dependencies.txt", impl->module.moduleDependencyPaths);
    readLines(cacheFolderModule / "Libraries.txt", impl->module.linkingLibraries);

    readLines(cacheFolderSetup / "Libraries.txt", impl->setup.linkingLibraries);
    readLines(cacheFolderSetup / "PublicIncludes.txt", impl->setup.publicIncludes);
    readLines(cacheFolderSetup / "PrivateIncludes.txt", impl->setup.privateIncludes);
    readLines(cacheFolderSetup / "PublicDefines.txt", impl->setup.publicDefines);
    readLines(cacheFolderSetup / "PrivateDefines.txt", impl->setup.privateDefines);

    std::ifstream moduleType(cacheFolderModule / "ModuleType.txt");
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
    std::filesystem::path uuidStorage = cacheFolderModule / "ModuleId.txt";
    if (Utility::GetCachedUUID(uuidStorage.string().c_str(), uuid))
    {
        impl->module.uuid = uuid;
    }

    std::ifstream mountList(cacheFolderModule / "Mounts.txt");
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
