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
}

#endif
