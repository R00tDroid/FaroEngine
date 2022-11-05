#include "VulkanSDKInfo.hpp"
#include <stdlib.h>

bool ReadEnvVariable(std::string variableName, std::string& value)
{
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

    return true;
}

std::vector<VulkanSDK> VulkanSDKs;
void FindVulkanSDKs()
{
    std::string path;
    if (ReadEnvVariable("VK_SDK_PATH", path))
    {
        VulkanSDKs.push_back({ path, "" });
    }

}

const std::vector<VulkanSDK>& GetVulkanSDKs()
{
    FindVulkanSDKs();
    return VulkanSDKs;
}
