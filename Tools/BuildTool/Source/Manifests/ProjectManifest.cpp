#include "ProjectManifest.hpp"
#include <fstream>
#include <Utility.hpp>
#include "ModuleManifest.hpp"

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

    std::ofstream moduleList(manifest->infoDirectory / "Modules.txt");
    for(std::filesystem::path& modulePath : manifest->projectModules)
    {
        moduleList << modulePath.string() << std::endl;
    }
    moduleList.close();

    return manifest;
}

ProjectManifest* ProjectManifest::LoadFromCache(std::filesystem::path path)
{
    ProjectManifest* manifest = new ProjectManifest(path);
    manifest->uuid = Utility::GetCachedUUID(manifest->infoDirectory / "ProjectId.txt");
    //TODO Load project info
    //TODO Load module list
    return manifest;
}

ProjectManifest::ProjectManifest(const std::filesystem::path& path): IManifest(path)
{}

bool ProjectManifest::ParseProject(picojson::object& rootObject)
{
    if (rootObject.find("Name") != rootObject.end())
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
                        std::filesystem::path relativePath = std::filesystem::proximate(path, manifestDirectory);
                        projectModules.push_back(relativePath);
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
