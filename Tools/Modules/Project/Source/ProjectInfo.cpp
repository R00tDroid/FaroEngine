#include "ProjectInfo.hpp"
#include <fstream>
#include <sstream>
#include <Utility.hpp>
#include <picojson.h>

struct ProjectManifest::Impl
{
    std::string projectName = "";
    std::vector<std::filesystem::path> modulesPaths;
    std::string uuid = "";
};

const char* ProjectManifest::projectManifestExtension()
{
    return ".faroproject.json";
}

ProjectManifest::ProjectManifest(const char* manifestLocation) : IManifest(manifestLocation)
{
    impl = new Impl();
}

ProjectManifest::~ProjectManifest()
{
    delete impl;
}

const char* ProjectManifest::projectName() const
{
    return impl->projectName.c_str();
}

const char* ProjectManifest::uuid() const
{
    return impl->uuid.c_str();
}

unsigned int ProjectManifest::modules() const
{
    return impl->modulesPaths.size();
}

const char* ProjectManifest::modulePath(unsigned int index) const
{
    return impl->modulesPaths[index].string().c_str();
}

bool ProjectManifest::load()
{
    return false;
}

bool ProjectManifest::loadCache()
{
    return false;
}

bool ProjectManifest::loadManifest()
{
    return false;
}

ProjectManifest* ProjectManifest::Parse(std::filesystem::path path)
{
    ProjectManifest* manifest = new ProjectManifest(path);

    std::ifstream fileStream(path);
    if (!fileStream.is_open())
    {
        Utility::PrintLine("Failed to open project file: " + path.string());
        delete manifest;
        return nullptr;
    }

    picojson::value rootValue;
    std::string error = picojson::parse(rootValue, fileStream);
    if (!error.empty())
    {
        Utility::Print("JSON parsing error: " + error);
        delete manifest;
        return nullptr;
    }

    if (!rootValue.is<picojson::object>())
    {
        Utility::PrintLine("Expected a JSON object as root of the project manifest");
        delete manifest;
        return nullptr;
    }

    picojson::object& rootObject = rootValue.get<picojson::object>();

    if (!manifest->ParseProject(rootObject))
    {
        delete manifest;
        return nullptr;
    }
    if (!manifest->ParseModules(rootObject))
    {
        delete manifest;
        return nullptr;
    }

    manifest->uuid = Utility::GetCachedUUID(manifest->infoDirectory / "ProjectId.txt");

    manifest->SaveCachedInfo();

    for (std::filesystem::path& modulePath : manifest->modulesPaths)
    {
        ModuleManifest::Parse(modulePath, manifest);
    }

    return manifest;
}

ProjectManifest* ProjectManifest::LoadFromCache(std::filesystem::path path)
{
    ProjectManifest* manifest = new ProjectManifest(path);
    manifest->uuid = Utility::GetCachedUUID(manifest->infoDirectory / "ProjectId.txt");

    manifest->LoadCachedInfo();

    for (std::filesystem::path& modulePath : manifest->modulesPaths)
    {
        ModuleManifest::LoadFromCache(modulePath, manifest);
    }

    return manifest;
}

ProjectManifest::ProjectManifest(const std::filesystem::path& path): IManifest(path)
{}

void ProjectManifest::SaveCachedInfo()
{
    std::ofstream moduleList(infoDirectory / "Modules.txt");
    for (std::filesystem::path& modulePath : modulesPaths)
    {
        moduleList << modulePath.string() << std::endl;
    }
    moduleList.close();

    std::ofstream projectNameFile(infoDirectory / "ProjectName.txt");
    projectNameFile << projectName;
    projectNameFile.close();
}

void ProjectManifest::LoadCachedInfo()
{
    std::ifstream moduleList(infoDirectory / "Modules.txt");
    std::string moduleFilePath;
    while (std::getline(moduleList, moduleFilePath))
    {
        modulesPaths.push_back(moduleFilePath);
    }
    moduleList.close();

    std::ifstream projectNameFile(infoDirectory / "ProjectName.txt");
    std::stringstream projectNameStream;
    projectNameStream << projectNameFile.rdbuf();
    projectNameFile.close();
    projectName = projectNameStream.str();
}

bool ProjectManifest::ParseProject(picojson::object& rootObject)
{
    if (rootObject.find("Name") != rootObject.end()) //TODO Save and load project name
    {
        picojson::value& value = rootObject["Name"];
        if (!value.is<std::string>())
        {
            Utility::PrintLine("Expected Name to be a string");
            return false;
        }
        projectName = value.get<std::string>();
    }

    return true;
}

bool ProjectManifest::ParseModules(picojson::object& rootObject)
{
    if (rootObject.find("Modules") != rootObject.end())
    {
        picojson::value& value = rootObject["Modules"];
        if (!value.is<picojson::array>())
        {
            Utility::PrintLine("Expected Modules to be an array");
            return false;
        }

        picojson::array& moduleArray = value.get<picojson::array>();
        for (picojson::value& moduleValue : moduleArray)
        {
            if (!moduleValue.is<std::string>())
            {
                Utility::PrintLine("Expected module path to be a string");
                return false;
            }

            std::filesystem::path moduleRoot = manifestDirectory / moduleValue.get<std::string>();
            moduleRoot.make_preferred();

            bool foundManifest = false;
            for (auto const& entry : std::filesystem::directory_iterator(moduleRoot))
            {
                if (std::filesystem::is_regular_file(entry))
                {
                    std::string path = entry.path().string().substr();
                    if (path.length() > ModuleManifest::moduleFileSuffix.length() && path.substr(path.length() - ModuleManifest::moduleFileSuffix.length()) == ModuleManifest::moduleFileSuffix)
                    {
                        modulesPaths.push_back(path);
                        foundManifest = true;
                        break;
                    }
                }
            }

            if (!foundManifest)
            {
                Utility::PrintLine("Could not find module manifest in: " + moduleRoot.string());
                return false;
            }
        }
    }
    return true;
}
