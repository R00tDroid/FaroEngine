#pragma once
#include "ManifestInterface.hpp"
#include <picojson.h>

class ProjectManifest : public IManifest
{
public:
    inline static std::string projectFileSuffix = ".faroproject.json";

    std::string projectName = "";
    std::vector<std::filesystem::path> modulesPaths;
    std::string uuid = "";
    
    // Parse the project manifest at the provided location. Returns nullptr if not able to parse the manifest.
    static ProjectManifest* Parse(std::filesystem::path path);

    // Load cached information about the manifest at the provided location. Returns nullptr if not able to load the cached information.
    static ProjectManifest* LoadFromCache(std::filesystem::path path);

protected:
    ProjectManifest(const std::filesystem::path& path);

private:
    bool ParseProject(picojson::object& rootObject);

    bool ParseModules(picojson::object& rootObject);
};
