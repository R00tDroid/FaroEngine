#include <gtest/gtest.h>
#include "Manifests/ProjectManifest.hpp"
#include "TestPaths.hpp"

TEST(ProjectManifest, ParseNothing)
{
    std::filesystem::path project = {};
    ProjectManifest* manifest = ProjectManifest::Parse(project);
    EXPECT_EQ(manifest, nullptr);
}

TEST(ProjectManifest, Parse)
{
    std::filesystem::path project = GetTestProjectManifest();
    ProjectManifest* manifest = ProjectManifest::Parse(project);
    EXPECT_NE(manifest, nullptr);
    EXPECT_STREQ(manifest->projectName.c_str(), "Test Project");
    EXPECT_EQ(manifest->modulesPaths.size(), 3);
}

TEST(ProjectManifest, LoadCached)
{
    std::filesystem::path project = GetTestProjectManifest();


    ProjectManifest* parsedManifest = ProjectManifest::Parse(project);
    EXPECT_NE(parsedManifest, nullptr);

    ProjectManifest* loadedManifest = ProjectManifest::LoadFromCache(project);
    EXPECT_NE(loadedManifest, nullptr);

    EXPECT_NE(loadedManifest, parsedManifest);

    EXPECT_STREQ(loadedManifest->projectName.c_str(), parsedManifest->projectName.c_str());
    EXPECT_EQ(loadedManifest->modulesPaths.size(), parsedManifest->modulesPaths.size());
}
