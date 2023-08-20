#pragma once
#include "ModuleManifest.hpp"

class IManifest
{
public:
    IManifest(std::filesystem::path manifestLocation) :
    manifestPath(manifestLocation),
    manifestDirectory(manifestPath.parent_path()),
    faroDirectory(manifestDirectory / ".Faro")
    {
        if (std::filesystem::exists(manifestPath)) 
        {
            Utility::EnsureDirectory(faroDirectory);
            Utility::HideFolder(faroDirectory);
        }
    }

    // Path to the manifest file
    std::filesystem::path manifestPath = "";
    // Path to the directory containing the manifest
    std::filesystem::path manifestDirectory = "";
    // Path to the hidden .Faro directory in which to generate intermediate files
    std::filesystem::path faroDirectory = "";
};
