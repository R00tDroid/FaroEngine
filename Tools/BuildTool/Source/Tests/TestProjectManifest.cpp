#if FaroToolTest
#include <gtest/gtest.h>
#include "Manifests/ProjectManifest.hpp"
#include "TestInfo.generated.hpp"

std::filesystem::path GetTestProjectRoot()
{
    std::filesystem::path path = std::filesystem::path(TestAppRoot) / "TestProject" / "TestProject.faroproject.json";
    path.make_preferred();
    return path;
}

TEST(Project, ParseManifest)
{
    std::filesystem::path project = GetTestProjectRoot();
    ProjectManifest* manifest = ProjectManifest::Parse(project);
    EXPECT_NE(manifest, nullptr);
    EXPECT_STREQ(manifest->projectName.c_str(), "Test Project");
    EXPECT_EQ(manifest->modulesPaths.size(), 3);
}
#endif
