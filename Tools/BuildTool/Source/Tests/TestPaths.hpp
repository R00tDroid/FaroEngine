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

inline std::filesystem::path GetTestModuleRoot(std::string moduleName)
{
    std::filesystem::path path = GetTestProjectRoot() / "Modules" / moduleName;
    path.make_preferred();
    return path;
}

inline std::filesystem::path GetTestModuleManifest(std::string moduleName)
{
    std::filesystem::path path = GetTestModuleRoot(moduleName) / (moduleName + ".module.json");
    path.make_preferred();
    return path;
}

#endif
