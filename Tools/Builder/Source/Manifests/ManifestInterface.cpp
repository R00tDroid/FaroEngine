#include "ManifestInterface.hpp"
#include <Utility.hpp>

IManifest::IManifest(const std::filesystem::path& manifestLocation):
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
