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

    ModuleManifest* parsedManifest = ModuleManifest::Parse(modulePath, project);
    EXPECT_NE(parsedManifest, nullptr);

    ModuleManifest* loadedManifest = ModuleManifest::LoadFromCache(modulePath, project);
    EXPECT_NE(loadedManifest, nullptr);

    EXPECT_NE(loadedManifest, parsedManifest);

    EXPECT_EQ(loadedManifest->project, parsedManifest->project);
    EXPECT_EQ(loadedManifest->sourceFiles.size(), parsedManifest->sourceFiles.size());
    EXPECT_EQ(loadedManifest->publicIncludes.size(), parsedManifest->publicIncludes.size());
    EXPECT_EQ(loadedManifest->privateIncludes.size(), parsedManifest->privateIncludes.size());
    EXPECT_EQ(loadedManifest->publicDefines.size(), parsedManifest->publicDefines.size());
    EXPECT_EQ(loadedManifest->privateDefines.size(), parsedManifest->privateDefines.size());
    EXPECT_EQ(loadedManifest->linkingLibraries.size(), parsedManifest->linkingLibraries.size());
    EXPECT_EQ(loadedManifest->moduleDependencies.size(), parsedManifest->moduleDependencies.size());
    EXPECT_EQ(loadedManifest->type, MT_Library);
    EXPECT_STRNE(loadedManifest->uuid.c_str(), "");
}

#endif
