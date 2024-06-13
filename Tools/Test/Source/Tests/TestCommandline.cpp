#include <gtest/gtest.h>
#include "Commandline.hpp"

TEST(CommandList, ParameterListNull)
{
    EXPECT_NO_FATAL_FAILURE(ParameterList parameters(0, nullptr));
}

TEST(CommandList, ParameterListInvalidCount)
{
    EXPECT_NO_FATAL_FAILURE(ParameterList parameters(10, nullptr));
}

#define ExpandCommand(commands) (int)std::size(commands), commands

TEST(CommandList, ParameterListCommands)
{
    const char* commands[] = { "dummyPath", "-command1", "-command2" };
    ParameterList parameters(ExpandCommand(commands));

    EXPECT_TRUE(parameters.Contains("command1"));
    EXPECT_TRUE(parameters.Contains("command2"));

    EXPECT_FALSE(parameters.Contains("-command1"));
    EXPECT_FALSE(parameters.Contains("-command2"));

    EXPECT_FALSE(parameters.Contains("Command1"));
    EXPECT_FALSE(parameters.Contains("command"));
}

TEST(CommandList, ParameterListArguments)
{
    const char* commands[] = { "dummyPath", "-command1", "-command2", "argument2.1", "-command and spaces", "argument with spaces" };
    ParameterList parameters(ExpandCommand(commands));

    EXPECT_TRUE(parameters.Contains("command1"));
    EXPECT_TRUE(parameters.Contains("command2"));
    EXPECT_TRUE(parameters.Contains("command and spaces"));

    EXPECT_FALSE(parameters.Contains("argument2.1"));

    EXPECT_FALSE(parameters.HasArguments("command1"));
    EXPECT_TRUE(parameters.HasArguments("command2"));
    EXPECT_TRUE(parameters.HasArguments("command and spaces"));

    EXPECT_EQ(parameters.CountArguments("command1"), 0);
    EXPECT_EQ(parameters.CountArguments("command2"), 1);
    EXPECT_EQ(parameters.CountArguments("command and spaces"), 1);
}

TEST(CommandList, ParameterListNoParameters)
{
    const char* commands[] = { "dummyPath" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_TRUE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 0);
    EXPECT_EQ(taskInfo.moduleList.size(), 0);
    EXPECT_EQ(taskInfo.projectManifest, nullptr);
    EXPECT_STREQ(taskInfo.buildPlatform.c_str(), "");
    EXPECT_STREQ(taskInfo.projectManifestPath.string().c_str(), "");
}

TEST(CommandList, ParameterListProjectInvalid)
{
    const char* commands[] = { "dummyPath", "-project" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_FALSE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_STREQ(taskInfo.projectManifestPath.string().c_str(), "");
}

TEST(CommandList, ParameterListPlatformNoArguments)
{
    const char* commands[] = { "dummyPath", "-platform" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_FALSE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_STREQ(taskInfo.buildPlatform.c_str(), "");
}

TEST(CommandList, ParameterListPlatform)
{
    const char* commands[] = { "dummyPath", "-platform", "windowsx64" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_TRUE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_STREQ(taskInfo.buildPlatform.c_str(), "windowsx64");
}

TEST(CommandList, ParameterListGenerate)
{
    const char* commands[] = { "dummyPath", "-project", "project/path/file.json", "-generate" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_TRUE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 1);
}

TEST(CommandList, ParameterListBuildNoPlatform)
{
    const char* commands[] = { "dummyPath", "-project", "project/path/file.json", "-build" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_FALSE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 0);
}

TEST(CommandList, ParameterListBuildNoConfig)
{
    const char* commands[] = { "dummyPath", "-project", "project/path/file.json", "-platform", "windows", "x64", "-build" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_FALSE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 0);
}

TEST(CommandList, ParameterListBuild)
{
    const char* commands[] = { "dummyPath", "-project", "project/path/file.json", "-platform", "windowsx64", "-debug", "-build" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_TRUE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 1);
}

TEST(CommandList, ParameterListBuildAndGenerate)
{
    const char* commands[] = { "dummyPath", "-project", "project/path/file.json", "-platform", "windowsx64", "-debug", "-build", "-generate" };
    ParameterList parameters(ExpandCommand(commands));

    TaskRunInfo taskInfo;
    std::vector<ITask*> tasks;

    EXPECT_TRUE(ParseParameters(parameters, tasks, taskInfo));
    EXPECT_EQ(tasks.size(), 2);
    EXPECT_STREQ(tasks[0]->GetTaskName().c_str(), "Generate");
    EXPECT_STREQ(tasks[1]->GetTaskName().c_str(), "Build");
}
