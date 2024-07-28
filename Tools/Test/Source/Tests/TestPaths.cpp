#include "TestPaths.hpp"
#include "TestInfo.generated.hpp"
#include "Manifests/ProjectManifest.hpp"

std::filesystem::path GetTestProjectRoot()
{
    std::filesystem::path path = std::filesystem::path(TestAppRoot) / "TestProject";
    path.make_preferred();
    return path;
}

std::filesystem::path GetTestProjectManifest()
{
    std::filesystem::path path = GetTestProjectRoot() / "TestProject.faroproject.json";
    path.make_preferred();
    return path;
}

std::filesystem::path GetTestModuleRoot(std::string moduleName)
{
    std::filesystem::path path = GetTestProjectRoot() / "Modules" / moduleName;
    path.make_preferred();
    return path;
}

std::filesystem::path GetTestModuleManifest(std::string moduleName)
{
    std::filesystem::path path = GetTestModuleRoot(moduleName) / (moduleName + ".module.json");
    path.make_preferred();
    return path;
}

ProjectManifest* LoadTestProject()
{
    std::filesystem::path project = GetTestProjectManifest();
    return ProjectManifest::Parse(project);
}
