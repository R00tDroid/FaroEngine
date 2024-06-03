#include "EmscriptenInfo.hpp"

#include <algorithm>

//TODO Remove duplicate from VK SDK
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

bool operator<(const EmscriptenSDK& a, const EmscriptenSDK& b)
{
    return std::tie(a.Root) < std::tie(b.Root);
}

bool operator==(const EmscriptenSDK& a, const EmscriptenSDK& b)
{
    return std::tie(a.Root) == std::tie(b.Root);
}

std::vector<EmscriptenSDK> EmscriptenSDKs;
const std::vector<EmscriptenSDK>& GetEmscriptenSDKs()
{
    std::string path;
    if (ReadEnvVariable("EMSDK", path))
    {
        EmscriptenSDKs.push_back({ path });
    }

    // Remove duplicates
    std::sort(EmscriptenSDKs.begin(), EmscriptenSDKs.end());
    EmscriptenSDKs.erase(unique(EmscriptenSDKs.begin(), EmscriptenSDKs.end()), EmscriptenSDKs.end());

    return EmscriptenSDKs;
}
