#pragma once
#if FaroToolTest
#include <filesystem>
#include "TestInfo.generated.hpp"

inline std::filesystem::path GetTestProjectRoot()
{
    std::filesystem::path path = std::filesystem::path(TestAppRoot) / "TestProject";
    path.make_preferred();
    return path;
}

inline std::filesystem::path GetTestProjectManifest()
{
    std::filesystem::path path = GetTestProjectRoot() / "TestProject.faroproject.json";
    path.make_preferred();
    return path;
}

#endif
