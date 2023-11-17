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
    ProjectManifest* project = GetProject();

    std::filesystem::path modulePath = {};
    ModuleManifest* manifest = ModuleManifest::Parse(modulePath, project);
    EXPECT_EQ(manifest, nullptr);
}

#endif
