#pragma once
#if FaroToolTest
#include <filesystem>

extern std::filesystem::path GetTestProjectRoot();

extern std::filesystem::path GetTestProjectManifest();

extern std::filesystem::path GetTestModuleRoot(std::string moduleName);

extern std::filesystem::path GetTestModuleManifest(std::string moduleName);

class ProjectManifest;
extern ProjectManifest* LoadTestProject();
#endif
