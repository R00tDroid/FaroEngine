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

TEST(ProjectManifest, ParseNothing)
{
    std::filesystem::path project = {};
    ProjectManifest* manifest = ProjectManifest::Parse(project);
    EXPECT_EQ(manifest, nullptr);
}

TEST(ProjectManifest, Parse)
{
    std::filesystem::path project = GetTestProjectRoot();
    ProjectManifest* manifest = ProjectManifest::Parse(project);
    EXPECT_NE(manifest, nullptr);
    EXPECT_STREQ(manifest->projectName.c_str(), "Test Project");
    EXPECT_EQ(manifest->modulesPaths.size(), 3);
}

TEST(ProjectManifest, LoadCached)
{
    std::filesystem::path project = GetTestProjectRoot();

    {
        ProjectManifest* manifest = ProjectManifest::Parse(project);
        EXPECT_NE(manifest, nullptr);
        EXPECT_STREQ(manifest->projectName.c_str(), "Test Project");
        EXPECT_EQ(manifest->modulesPaths.size(), 3);
    }

    {
        ProjectManifest* manifest = ProjectManifest::LoadFromCache(project);
        EXPECT_NE(manifest, nullptr);
        EXPECT_STREQ(manifest->projectName.c_str(), "Test Project");
        EXPECT_EQ(manifest->modulesPaths.size(), 3);
    }
}
#endif
