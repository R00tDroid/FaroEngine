#pragma once
#include <filesystem>
#include <Utility.hpp>

class IManifest
{
public:
    IManifest(const std::filesystem::path& manifestLocation) :
        manifestPath(manifestLocation),
        manifestDirectory(manifestPath.parent_path()),
        faroDirectory(manifestDirectory / ".Faro"),
        infoDirectory(faroDirectory / "Info")
    {
        if (std::filesystem::exists(manifestPath)) 
        {
            Utility::EnsureDirectory(faroDirectory);
            Utility::EnsureDirectory(infoDirectory);
            Utility::HideFolder(faroDirectory);
        }
    }

    // Path to the manifest file
    std::filesystem::path manifestPath = "";
    // Path to the directory containing the manifest
    std::filesystem::path manifestDirectory = "";
    // Path to the hidden .Faro directory in which to generate intermediate files
    std::filesystem::path faroDirectory = "";
    // Path to the hidden .Faro/Info directory in which to generate information cache files
    std::filesystem::path infoDirectory = "";
};
