#include "VulkanSDKInfo.hpp"
#include "Utility.hpp"
#include <algorithm>
#include <stdlib.h>

std::vector<VulkanSDK> VulkanSDKs;
void FindVulkanSDKs()
{
    std::string path;
    if (Utility::ReadEnvVariable("VK_SDK_PATH", path))
    {
        VulkanSDKs.push_back({ path });
    }

    if (Utility::ReadEnvVariable("VULKAN_SDK", path))
    {
        VulkanSDKs.push_back({ path });
    }

    // Remove duplicates
    std::sort(VulkanSDKs.begin(), VulkanSDKs.end());
    VulkanSDKs.erase(unique(VulkanSDKs.begin(), VulkanSDKs.end()), VulkanSDKs.end());
}

bool operator<(const VulkanSDK& a, const VulkanSDK& b)
{
    return std::tie(a.Root) < std::tie(b.Root);
}

bool operator==(const VulkanSDK& a, const VulkanSDK& b)
{
    return std::tie(a.Root) == std::tie(b.Root);
}

const std::vector<VulkanSDK>& GetVulkanSDKs()
{
    FindVulkanSDKs();
    return VulkanSDKs;
}
