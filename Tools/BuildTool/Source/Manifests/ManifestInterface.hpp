#pragma once
#include <filesystem>

class IManifest
{
public:
    IManifest(const std::filesystem::path& manifestLocation);

    // Path to the manifest file
    std::filesystem::path manifestPath = "";
    // Path to the directory containing the manifest
    std::filesystem::path manifestDirectory = "";
    // Path to the hidden .Faro directory in which to generate intermediate files
    std::filesystem::path faroDirectory = "";
    // Path to the hidden .Faro/Info directory in which to generate information cache files
    std::filesystem::path infoDirectory = "";
};
