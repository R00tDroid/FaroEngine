#include "ManifestInterface.hpp"
#include <filesystem>
#include <Utility.hpp>

struct IManifest::Impl
{
    std::filesystem::path manifestPath = "";
    std::filesystem::path manifestDirectory = "";
    std::filesystem::path faroDirectory = "";
    std::filesystem::path cacheDirectory = "";
};

IManifest::IManifest(const char* manifestLocation)
{
    impl = new Impl();

    impl->manifestPath = manifestLocation;

    impl->manifestDirectory = impl->manifestPath.parent_path();
    impl->faroDirectory = impl->manifestDirectory / ".Faro";
    impl->cacheDirectory = impl->faroDirectory / "Cache";

    if (std::filesystem::exists(impl->manifestPath))
    {
        Utility::EnsureDirectory(impl->faroDirectory);
        Utility::HideFolder(impl->faroDirectory);
        Utility::EnsureDirectory(impl->cacheDirectory);
    }
}

IManifest::~IManifest()
{
    delete impl;
}

const char* IManifest::manifestPath() const
{
    return impl->manifestPath.string().c_str();
}

const char* IManifest::manifestDirectory() const
{
    return impl->manifestDirectory.string().c_str();
}

const char* IManifest::faroDirectory() const
{
    return impl->faroDirectory.string().c_str();
}

const char* IManifest::cacheDirectory() const
{
    return impl->cacheDirectory.string().c_str();
}
