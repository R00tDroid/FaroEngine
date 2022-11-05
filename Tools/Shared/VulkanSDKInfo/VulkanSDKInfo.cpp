#include "VulkanSDKInfo.hpp"

#include <algorithm>
#include <stdlib.h>

bool ReadEnvVariable(std::string variableName, std::string& value)
{
#ifdef WIN32
    size_t size = 0;
    if (getenv_s(&size, nullptr, 0, variableName.c_str()) != 0)
    {
        return false;
    }
    value.resize(size);
    if (getenv_s(&size, value.data(), size, variableName.c_str()) != 0)
    {
        return false;
    }
#else
    value = std::string(getenv(variableName.c_str()));
#endif

    // Remove null-terminator at end of string
    if (value.back() == '\0')
    {
        value.pop_back();
    }

    return true;
}

std::vector<VulkanSDK> VulkanSDKs;
void FindVulkanSDKs()
{
    std::string path;
    if (ReadEnvVariable("VK_SDK_PATH", path))
    {
        VulkanSDKs.push_back({ path, ""});
    }

    if (ReadEnvVariable("VULKAN_SDK", path))
    {
        VulkanSDKs.push_back({ path, "" });
    }

    // Remove duplicates
    std::sort(VulkanSDKs.begin(), VulkanSDKs.end());
    VulkanSDKs.erase(unique(VulkanSDKs.begin(), VulkanSDKs.end()), VulkanSDKs.end());
}

bool operator<(const VulkanSDK& a, const VulkanSDK& b)
{
    return std::tie(a.Root, a.Version) < std::tie(b.Root, b.Version);
}

bool operator==(const VulkanSDK& a, const VulkanSDK& b)
{
    return std::tie(a.Root, a.Version) == std::tie(b.Root, b.Version);
}

const std::vector<VulkanSDK>& GetVulkanSDKs()
{
    FindVulkanSDKs();
    return VulkanSDKs;
}
