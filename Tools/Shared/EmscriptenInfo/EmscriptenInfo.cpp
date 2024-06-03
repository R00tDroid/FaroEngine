#include "EmscriptenInfo.hpp"
#include "../Utils/Utility.hpp"
#include <algorithm>

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
    if (Utility::ReadEnvVariable("EMSDK", path))
    {
        EmscriptenSDKs.push_back({ path });
    }

    // Remove duplicates
    std::sort(EmscriptenSDKs.begin(), EmscriptenSDKs.end());
    EmscriptenSDKs.erase(unique(EmscriptenSDKs.begin(), EmscriptenSDKs.end()), EmscriptenSDKs.end());

    return EmscriptenSDKs;
}
