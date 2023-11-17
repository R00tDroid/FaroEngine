#pragma once
#if FaroToolTest
#include <filesystem>
#include "TestInfo.generated.hpp"

inline std::filesystem::path GetTestProjectRoot()
{
    std::filesystem::path path = std::filesystem::path(TestAppRoot) / "TestProject" / "TestProject.faroproject.json";
    path.make_preferred();
    return path;
}

#endif
