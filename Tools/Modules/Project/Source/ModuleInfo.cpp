#include "ModuleInfo.hpp"
#include <Utility.hpp>
#include <fstream>
#include <sstream>

struct ModuleManifest::Impl
{
    std::string name;
    std::filesystem::path solutionDirectory;

    std::vector<std::string> linkingLibraries;
    std::vector<std::string> publicDefines;
    std::vector<std::string> privateDefines;

    ModuleType type = MT_Library;

    std::vector<std::filesystem::path> moduleDependencies;

    std::vector<std::filesystem::path> privateIncludes;
    std::vector<std::filesystem::path> publicIncludes;

    std::vector<std::filesystem::path> sourceFiles;

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

bool ReadManifestFile(std::filesystem::path& path, picojson::object& rootObject)
{
    if (!std::filesystem::exists(path))
    {
        Utility::PrintLine("Failed to find module manifest: " + path.string());
        return false;
    }

    std::ifstream fileStream(path);
    if (!fileStream.is_open())
    {
        Utility::PrintLine("Failed to open module manifest: " + path.string());
        return false;
    }

    picojson::value rootValue;
    std::string error = picojson::parse(rootValue, fileStream);
    if (!error.empty())
    {
        Utility::Print("JSON parsing error: " + error);
        return false;
    }

    if (!rootValue.is<picojson::object>())
    {
        Utility::PrintLine("Expected a JSON object as root of the module manifest");
        return false;
    }

    rootObject = rootValue.get<picojson::object>();

    return true;
}

ModuleManifest* ModuleManifest::Parse(std::filesystem::path path, ProjectManifest* project)
{
    if (project == nullptr)
    {
        return nullptr;
    }

    picojson::object rootObject;
    if (!ReadManifestFile(path, rootObject))
    {
        return nullptr;
    }

    ModuleManifest* manifest = new ModuleManifest(path);
    manifest->project = project;

    if (!manifest->ParseSourceFiles(rootObject) || 
        !manifest->ParseDependencies(rootObject) ||
        !manifest->ParseIncludeDirectories(rootObject, "PublicIncludeDirectories", manifest->publicIncludes) ||
        !manifest->ParseIncludeDirectories(rootObject, "PrivateIncludeDirectories", manifest->privateIncludes) ||
        !manifest->ParseDefines(rootObject, "PublicDefines", manifest->publicDefines) ||
        !manifest->ParseDefines(rootObject, "PrivateDefines", manifest->privateDefines) ||
        !manifest->ParseModuleType(rootObject) ||
        !manifest->ParseSolutionLocation(rootObject) ||
        !manifest->ParseLinkerLibraries(rootObject) ||
        !manifest->ParsePlatformFilter(rootObject) ||
        !manifest->ParseMounts(rootObject))
    {
        delete manifest;
        return nullptr;
    }

    manifest->uuid = Utility::GetCachedUUID(manifest->infoDirectory / "ModuleId.txt");

    manifest->SaveCache();

    return manifest;
}

ModuleManifest* ModuleManifest::LoadFromCache(std::filesystem::path path, ProjectManifest* project)
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

ModuleManifest* ModuleManifest::GetLoadedModule(std::filesystem::path path)
{
    if (loadedModules.find(path) != loadedModules.end())
    {
        return loadedModules[path];
    }
    else
    {
        return nullptr;
    }
}

std::vector<std::filesystem::path> ModuleManifest::GetPublicIncludeTree() const
{
    std::vector<std::filesystem::path> result = publicIncludes;
    for (const std::filesystem::path& dependencyPath : moduleDependencies)
    {
        ModuleManifest* dependency = GetLoadedModule(dependencyPath);
        for (std::filesystem::path& path : dependency->GetPublicIncludeTree())
        {
            result.push_back(path);
        }
    }
    return result;
}

std::vector<std::filesystem::path> ModuleManifest::GetModuleIncludeDirectories() const
{
    std::vector<std::filesystem::path> result = GetPublicIncludeTree();
    for (const std::filesystem::path& path : privateIncludes)
    {
        result.push_back(path);
    }
    return result;
}

std::vector<std::string> ModuleManifest::GetPublicDefineTree() const
{
    std::vector<std::string> result = publicDefines;
    for (const std::filesystem::path& dependencyPath : moduleDependencies)
    {
        ModuleManifest* dependency = GetLoadedModule(dependencyPath);
        for (std::string& path : dependency->GetPublicDefineTree())
        {
            result.push_back(path);
        }
    }
    return result;
}

std::vector<std::string> ModuleManifest::GetModuleDefines() const
{
    std::vector<std::string> result = GetPublicDefineTree();
    for (const std::string& define : privateDefines)
    {
        result.push_back(define);
    }
    return result;
}

std::string ModuleManifest::GetModuleName(const std::filesystem::path& path)
{
    std::string result = path.filename().string();
    result.erase(result.length() - moduleFileSuffix.length());
    return result;
}

std::set<ModuleManifest*> ModuleManifest::GetDependencies()
{
    std::set<ModuleManifest*> dependencies;

    for (std::filesystem::path& dependencyPath : moduleDependencies)
    {
        ModuleManifest* dependency = GetLoadedModule(dependencyPath);
        dependencies.insert(dependency);
    }

    return dependencies;
}

std::set<ModuleManifest*> ModuleManifest::GetDependencyTree()
{
    std::set<ModuleManifest*> dependencies = GetDependencies();

    for (ModuleManifest* dependency : dependencies)
    {
        for (ModuleManifest* childDependency : dependency->GetDependencyTree())
        {
            dependencies.insert(childDependency);
        }
    }

    return dependencies;
}

bool ModuleManifest::IsCompatible(BuildPlatform* target) const
{
    return Utility::MatchWildcard(target->platformName, platformFilter);
}

bool ModuleManifest::ParseSourceFiles(picojson::object& rootObject)
{
    std::vector<std::filesystem::path> filters;
    if (rootObject.find("SourceFilters") != rootObject.end())
    {
        picojson::value& value = rootObject["SourceFilters"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected SourceFilters to be an array");
            return false;
        }

        picojson::array& filterArray = value.get<picojson::array>();
        for (picojson::value& filterValue : filterArray)
        {
            if (!filterValue.is<std::string>())
            {
                Utility::PrintLine("Expected filter to be a string");
                return false;
            }

            std::filesystem::path filter = manifestDirectory / filterValue.get<std::string>();
            filter = std::filesystem::weakly_canonical(filter);
            filter.make_preferred();
            filters.push_back(filter);

            while (true)
            {
                std::string string = filter.string();

                size_t index = string.find("**");
                if (index == std::string::npos) break;

                string[index] = '.';
                string.erase(index + 1, 1);

                filter = std::filesystem::weakly_canonical(string);
                filters.push_back(filter);
            }
        }
    }

    for (std::filesystem::path& filter : filters)
    {
        for (auto& file : glob::rglob(filter.string()))
        {
            sourceFiles.push_back(file);
        }
    }

    if (rootObject.find("GeneratedFiles") != rootObject.end())
    {
        picojson::value& value = rootObject["GeneratedFiles"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected GeneratedFiles to be an array");
            return false;
        }

        picojson::array& filterArray = value.get<picojson::array>();
        for (picojson::value& filterValue : filterArray)
        {
            if (!filterValue.is<std::string>())
            {
                Utility::PrintLine("Expected file path to be a string");
                return false;
            }

            std::filesystem::path filter = manifestDirectory / filterValue.get<std::string>();
            filter = std::filesystem::weakly_canonical(filter);
            filter.make_preferred();

            sourceFiles.push_back(filter);
        }
    }

    std::sort(sourceFiles.begin(), sourceFiles.end());
    sourceFiles.erase(std::unique(sourceFiles.begin(), sourceFiles.end()), sourceFiles.end());

    return true;
}

bool ModuleManifest::ParseDependencies(picojson::object& rootObject)
{
    moduleDependencies = {};

    if (rootObject.find("Dependencies") != rootObject.end())
    {
        picojson::value& value = rootObject["Dependencies"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected Dependencies to be an array");
            return false;
        }

        picojson::array& dependencyArray = value.get<picojson::array>();
        for (picojson::value& dependecyValue : dependencyArray)
        {
            if (!dependecyValue.is<std::string>())
            {
                Utility::PrintLine("Expected dependency to be a string");
                return false;
            }

            std::string dependencyName = dependecyValue.get<std::string>();
            for (auto& it : loadedModules)
            {
                if (dependencyName == GetModuleName(it.first))
                {
                    moduleDependencies.push_back(it.first);
                    break;
                }
            }
        }
    }

    return true;
}

bool ModuleManifest::ParseIncludeDirectories(picojson::object& rootObject, std::string tag, std::vector<std::filesystem::path>& output)
{
    output = {};

    if (rootObject.find(tag) != rootObject.end())
    {
        picojson::value& value = rootObject[tag];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected " + tag + " to be an array");
            return false;
        }

        picojson::array& includeArray = value.get<picojson::array>();
        for (picojson::value& includeValue : includeArray)
        {
            if (!includeValue.is<std::string>())
            {
                Utility::PrintLine("Expected include path to be a string");
                return false;
            }

            output.push_back(std::filesystem::weakly_canonical(manifestDirectory / includeValue.get<std::string>()));
        }
    }

    return true;
}

bool ModuleManifest::ParseDefines(picojson::object& rootObject, std::string tag, std::vector<std::string>& output)
{
    output = {};

    if (rootObject.find(tag) != rootObject.end())
    {
        picojson::value& value = rootObject[tag];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected " + tag + " to be an array");
            return false;
        }

        picojson::array& defineArray = value.get<picojson::array>();
        for (picojson::value& defineValue : defineArray)
        {
            if (!defineValue.is<std::string>())
            {
                Utility::PrintLine("Expected define to be a string");
                return false;
            }

            output.push_back(defineValue.get<std::string>());
        }
    }

    return true;
}

bool ModuleManifest::ParseModuleType(picojson::object& rootObject)
{
    solutionLocation = "";

    if (rootObject.find("ModuleType") != rootObject.end())
    {
        picojson::value& value = rootObject["ModuleType"];
        if (!value.is<std::string>())
        {
            Utility::PrintLine("Expected ModuleType to be a string");
            return false;
        }

        std::string typeString = value.get<std::string>();

        if (typeString == "Executable")
        {
            type = MT_Executable;
        }
        else if (typeString == "Library")
        {
            type = MT_Library;
        }
        else
        {
            Utility::PrintLine("Unknown module type: " + typeString + ". Should be Executable/Library.");
            return false;
        }
    }

    return true;
}

bool ModuleManifest::ParseSolutionLocation(picojson::object& rootObject)
{
    solutionLocation = "";

    if (rootObject.find("SolutionLocation") != rootObject.end())
    {
        picojson::value& value = rootObject["SolutionLocation"];
        if (!value.is<std::string>())
        {
            Utility::PrintLine("Expected SolutionLocation to be a string");
            return false;
        }

        solutionLocation = value.get<std::string>();
    }

    solutionLocation = std::filesystem::weakly_canonical(solutionLocation);

    return true;
}

bool ModuleManifest::ParseLinkerLibraries(picojson::object& rootObject)
{
    linkingLibraries = {};

    if (rootObject.find("Libraries") != rootObject.end())
    {
        picojson::value& value = rootObject["Libraries"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected Libraries to be an array");
            return false;
        }

        picojson::array& libArray = value.get<picojson::array>();
        for (picojson::value& libValue : libArray)
        {
            if (!libValue.is<std::string>())
            {
                Utility::PrintLine("Expected include path to be a string");
                return false;
            }

            linkingLibraries.push_back(libValue.get<std::string>());
        }
    }

    return true;
}

bool ModuleManifest::ParsePlatformFilter(picojson::object& rootObject)
{
    platformFilter = "*";

    if (rootObject.find("PlatformFilter") != rootObject.end())
    {
        picojson::value& value = rootObject["PlatformFilter"];
        if (!value.is<std::string>())
        {
            Utility::PrintLine("Expected PlatformFilter to be a string");
            return false;
        }

        platformFilter = value.get<std::string>();
    }

    return true;
}

bool ModuleManifest::ParseMounts(picojson::object& rootObject)
{
    folderMounts = {};

    if (rootObject.find("Mounts") != rootObject.end())
    {
        picojson::value& value = rootObject["Mounts"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected Mounts to be an array");
            return false;
        }

        picojson::array& mountArray = value.get<picojson::array>();
        for (picojson::value& mountValue : mountArray)
        {
            if (!mountValue.is<picojson::object>())
            {
                Utility::PrintLine("Expected mount to be a object");
                return false;
            }

            picojson::object& mountObject = mountValue.get<picojson::object>();
            if (mountObject.find("Location") == mountObject.end())
            {
                Utility::PrintLine("Missing Location in mount definition");
                return false;
            }
            picojson::value& locationValue = mountObject["Location"];
            if (!locationValue.is<std::string>())
            {
                Utility::PrintLine("Expected Location to be a string");
                return false;
            }

            if (mountObject.find("MountPoint") == mountObject.end())
            {
                Utility::PrintLine("Missing MountPoint in mount definition");
                return false;
            }
            picojson::value& pointValue = mountObject["MountPoint"];
            if (!pointValue.is<std::string>())
            {
                Utility::PrintLine("Expected MountPoint be a string");
                return false;
            }

            folderMounts.push_back({ std::filesystem::weakly_canonical(manifestDirectory / locationValue.get<std::string>()), pointValue.get<std::string>() });
        }
    }

    return true;
}

void ModuleManifest::SaveCache()
{
    std::ofstream filesList(infoDirectory / "Source.txt");
    for (std::filesystem::path& path : sourceFiles)
    {
        filesList << path.string() << "\n";
    }
    filesList.close();

    std::ofstream publicIncludeList(infoDirectory / "PublicIncludes.txt");
    for (std::filesystem::path& path : publicIncludes)
    {
        publicIncludeList << path.string() << "\n";
    }
    publicIncludeList.close();

    std::ofstream privateIncludeList(infoDirectory / "PrivateIncludes.txt");
    for (std::filesystem::path& path : privateIncludes)
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
    for (std::filesystem::path dependency : moduleDependencies)
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
