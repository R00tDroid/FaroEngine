#if FaroToolTest
#include <gtest/gtest.h>
#include "Manifests/ModuleManifest.hpp"
#include "Manifests/ProjectManifest.hpp"
#include "TestPaths.hpp"

ProjectManifest* GetProject()
{
    std::filesystem::path project = GetTestProjectManifest();
    return ProjectManifest::Parse(project);
}

TEST(ModuleManifest, ParseNothing)
{
    ModuleManifest* manifest = ModuleManifest::Parse({}, nullptr);
    EXPECT_EQ(manifest, nullptr);
}

TEST(ModuleManifest, ParseNoPath)
{
    ProjectManifest* project = GetProject();

    std::filesystem::path modulePath = {};
    ModuleManifest* manifest = ModuleManifest::Parse(modulePath, project);
    EXPECT_EQ(manifest, nullptr);
}

TEST(ModuleManifest, Parse)
{
    ProjectManifest* project = GetProject();
    std::filesystem::path modulePath = project->modulesPaths[0];
    EXPECT_STREQ(modulePath.filename().string().c_str(), "ModuleA.module.json");

    ModuleManifest* manifest = ModuleManifest::Parse(modulePath, project);
    EXPECT_NE(manifest, nullptr);
    EXPECT_EQ(manifest->project, project);
    EXPECT_EQ(manifest->sourceFiles.size(), 2);
    EXPECT_EQ(manifest->publicIncludes.size(), 1);
    EXPECT_EQ(manifest->privateIncludes.size(), 0);
    EXPECT_EQ(manifest->publicDefines.size(), 0);
    EXPECT_EQ(manifest->privateDefines.size(), 0);
    EXPECT_EQ(manifest->linkingLibraries.size(), 0);
    EXPECT_EQ(manifest->moduleDependencies.size(), 0);
    EXPECT_EQ(manifest->type, MT_Library);
    EXPECT_STRNE(manifest->uuid.c_str(), "");
}

TEST(ModuleManifest, LoadCached)
{
    ProjectManifest* project = GetProject();
    std::filesystem::path modulePath = project->modulesPaths[0];
    EXPECT_STREQ(modulePath.filename().string().c_str(), "ModuleA.module.json");

    {
        ModuleManifest* manifest = ModuleManifest::LoadFromCache(modulePath, project);
        EXPECT_NE(manifest, nullptr);
    }

    {
        ModuleManifest* manifest = ModuleManifest::LoadFromCache(modulePath, project);
        EXPECT_NE(manifest, nullptr);
        EXPECT_EQ(manifest->project, project);
        EXPECT_EQ(manifest->sourceFiles.size(), 2);
        EXPECT_EQ(manifest->publicIncludes.size(), 1);
        EXPECT_EQ(manifest->privateIncludes.size(), 0);
        EXPECT_EQ(manifest->publicDefines.size(), 0);
        EXPECT_EQ(manifest->privateDefines.size(), 0);
        EXPECT_EQ(manifest->linkingLibraries.size(), 0);
        EXPECT_EQ(manifest->moduleDependencies.size(), 0);
        EXPECT_EQ(manifest->type, MT_Library);
        EXPECT_STRNE(manifest->uuid.c_str(), "");
    }
}

#endif
