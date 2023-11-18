#if FaroToolTest
#include <gtest/gtest.h>
#include "Manifests/ModuleManifest.hpp"
#include "Manifests/ProjectManifest.hpp"
#include "FileTree.hpp"
#include "TestPaths.hpp"

ProjectManifest* GetTestProject()
{
    std::filesystem::path project = GetTestProjectManifest();
    return ProjectManifest::Parse(project);
}

TEST(FileTree, MissingModule)
{
    FileTree tree(nullptr);
    EXPECT_NO_FATAL_FAILURE(tree.Parse());
}

TEST(FileTree, NoDependencies)
{
    ProjectManifest* project = GetTestProject();
    EXPECT_NE(project, nullptr);

    ModuleManifest* moduleA = ModuleManifest::GetLoadedModule(GetTestModuleManifest("ModuleA"));
    EXPECT_NE(moduleA, nullptr);

    moduleA->fileTree.Parse();

    EXPECT_EQ(moduleA->fileTree.GetTree(GetTestModuleRoot("ModuleA") / "Source" / "ModuleA.hpp").size(), 0);
    EXPECT_EQ(moduleA->fileTree.GetTree(GetTestModuleRoot("ModuleA") / "Source" / "ModuleA.cpp").size(), 1);
}

TEST(FileTree, WithDependencies)
{
    ProjectManifest* project = GetTestProject();
    EXPECT_NE(project, nullptr);

    ModuleManifest* moduleC = ModuleManifest::GetLoadedModule(GetTestModuleManifest("ModuleC"));
    EXPECT_NE(moduleC, nullptr);

    moduleC->fileTree.Parse();

    EXPECT_EQ(moduleC->fileTree.GetTree(GetTestModuleRoot("ModuleC") / "Source" / "ModuleC.hpp").size(), 0);
    EXPECT_EQ(moduleC->fileTree.GetTree(GetTestModuleRoot("ModuleC") / "Source" / "ModuleC.cpp").size(), 3);
}
#endif
