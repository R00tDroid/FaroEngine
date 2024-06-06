#pragma once
#include <string>
#include <vector>

struct EmscriptenSDK
{
    std::string Root;
};

extern const std::vector<EmscriptenSDK>& GetEmscriptenSDKs();
