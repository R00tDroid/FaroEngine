#include "ProjectInfo.hpp"
#include "ModuleInfo.hpp"
#include <fstream>
#include <sstream>
#include <Utility.hpp>
#include <picojson.h>

struct ProjectManifest::Impl
{
    std::string projectName = "";
    std::vector<ModuleManifest*> modules;
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
    return static_cast<unsigned int>(impl->modules.size());
}

ModuleManifest* ProjectManifest::module(unsigned int index) const
{
    return impl->modules[index];
}

bool parseProject(ProjectManifest::Impl& impl, picojson::object& rootObject)
{
    if (rootObject.find("Name") != rootObject.end()) //TODO Save and load project name
    {
        picojson::value& value = rootObject["Name"];
        if (!value.is<std::string>())
        {
            Utility::PrintLine("Expected Name to be a string");
            return false;
        }
        impl.projectName = value.get<std::string>();
    }

    return true;
}

bool parseModules(ProjectManifest::Impl& impl, ProjectManifest& project, picojson::object& rootObject)
{
    for (ModuleManifest* module : impl.modules)
    {
        delete module;
    }
    impl.modules.clear();

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

            std::filesystem::path moduleRoot = std::filesystem::path(project.manifestDirectory()) / moduleValue.get<std::string>();
            moduleRoot.make_preferred();

            bool foundManifest = false;
            for (auto const& entry : std::filesystem::directory_iterator(moduleRoot))
            {
                if (std::filesystem::is_regular_file(entry))
                {
                    std::string extension(ModuleManifest::moduleManifestExtension());

                    std::string path = entry.path().string().substr();
                    if (path.length() > extension.length() && path.substr(path.length() - extension.length()) == extension)
                    {
                        impl.modules.push_back(new ModuleManifest(path.c_str()));
                        foundManifest = true;
                        break;
                    }
                }
            }

            if (!foundManifest)
            {
                Utility::PrintLine("Could not find module manifest in: " + moduleRoot.string() + ". Should have the following extension: \"" + ModuleManifest::moduleManifestExtension() + "\"");
                return false;
            }
        }
    }
    return true;
}


bool ProjectManifest::configure()
{
    std::ifstream fileStream(manifestPath());

    if (!fileStream.is_open())
    {
        Utility::PrintLine("Failed to open project file: " + std::string(manifestPath()));
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
        Utility::PrintLine("Expected a JSON object as root of the project manifest");
        return false;
    }

    picojson::object& rootObject = rootValue.get<picojson::object>();

    if (!parseProject(*impl, rootObject))
    {
        return false;
    }
    if (!parseModules(*impl, *this, rootObject))
    {
        return false;
    }

    char uuid[UUID_LENGTH];
    std::filesystem::path uuidStorage = std::filesystem::path(cacheDirectory()) / "ProjectId.txt";
    if (Utility::GetCachedUUID(uuidStorage.string().c_str(), uuid))
    {
        impl->uuid = uuid;
    }

    std::ofstream moduleList(std::filesystem::path(cacheDirectory()) / "Modules.txt");
    for (ModuleManifest* module : impl->modules)
    {
        moduleList << module->manifestPath() << std::endl;
    }
    moduleList.close();

    std::ofstream projectNameFile(std::filesystem::path(cacheDirectory()) / "ProjectName.txt");
    projectNameFile << impl->projectName;
    projectNameFile.close();

    return true;
}

bool ProjectManifest::load()
{
    //TODO Check if cache is available

    impl->modules.clear();

    std::filesystem::path cacheDir(cacheDirectory());

    std::ifstream moduleList(cacheDir / "Modules.txt");
    std::string moduleFilePath;
    while (std::getline(moduleList, moduleFilePath))
    {
        impl->modules.push_back(new ModuleManifest(moduleFilePath.c_str()));
    }
    moduleList.close();

    std::ifstream projectNameFile(cacheDir / "ProjectName.txt");
    std::stringstream projectNameStream;
    projectNameStream << projectNameFile.rdbuf();
    projectNameFile.close();
    impl->projectName = projectNameStream.str();

    return true;
}
