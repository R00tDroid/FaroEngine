#include "ManifestInterface.hpp"
#include <filesystem>
#include <Utility.hpp>

struct IManifest::Impl
{
    std::string manifestPath = "";
    std::string manifestDirectory = "";
    std::string faroDirectory = "";
    std::string cacheDirectory = "";
};

IManifest::IManifest(const char* manifestLocation)
{
    impl = new Impl();

    std::filesystem::path manifestPath(manifestLocation);
    manifestPath.make_preferred();
    manifestPath = weakly_canonical(manifestPath);
    impl->manifestPath = manifestPath.string();

    std::filesystem::path manifestDirectory = manifestPath.parent_path();
    impl->manifestDirectory = manifestDirectory.string();
    std::filesystem::path faroDirectory = manifestDirectory / ".Faro";
    impl->faroDirectory = faroDirectory.string();
    impl->cacheDirectory = (faroDirectory / "Cache").string();

    if (std::filesystem::exists(impl->manifestPath))
    {
        Utility::EnsureDirectory(impl->faroDirectory.c_str());
        Utility::HideFolder(impl->faroDirectory.c_str());
        Utility::EnsureDirectory(impl->cacheDirectory.c_str());
    }
}

IManifest::~IManifest()
{
    delete impl;
}

const char* IManifest::manifestPath() const
{
    return impl->manifestPath.c_str();
}

const char* IManifest::manifestDirectory() const
{
    return impl->manifestDirectory.c_str();
}

const char* IManifest::faroDirectory() const
{
    return impl->faroDirectory.c_str();
}

const char* IManifest::cacheDirectory() const
{
    return impl->cacheDirectory.c_str();
}
