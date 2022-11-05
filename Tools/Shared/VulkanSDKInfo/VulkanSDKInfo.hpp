#pragma once
#include <string>
#include <vector>

struct VulkanSDK
{
    std::string Root;
    std::string Version;
};

extern const std::vector<VulkanSDK>& GetVulkanSDKs();
