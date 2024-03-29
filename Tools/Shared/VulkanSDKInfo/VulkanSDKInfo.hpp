#pragma once
#include <string>
#include <vector>

struct VulkanSDK
{
    std::string Root;
};

extern bool operator<(const VulkanSDK& a, const VulkanSDK& b);
extern bool operator==(const VulkanSDK& a, const VulkanSDK& b);

extern const std::vector<VulkanSDK>& GetVulkanSDKs();
