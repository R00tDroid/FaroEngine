#if FaroToolTest
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
    const char* commands[] = { "dummyPath", "-command1", "-command2", "argument2.1" };
    ParameterList parameters(ExpandCommand(commands));

    EXPECT_TRUE(parameters.Contains("command1"));
    EXPECT_TRUE(parameters.Contains("command2"));

    EXPECT_FALSE(parameters.Contains("argument2.1"));

    EXPECT_FALSE(parameters.HasArguments("command1"));
    EXPECT_TRUE(parameters.HasArguments("command2"));

    EXPECT_EQ(parameters.CountArguments("command1"), 0);
    EXPECT_EQ(parameters.CountArguments("command2"), 1);
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
    EXPECT_STREQ(taskInfo.buildArchitecture.c_str(), "");
    EXPECT_STREQ(taskInfo.buildPlatform.c_str(), "");
    EXPECT_STREQ(taskInfo.projectManifestPath.string().c_str(), "");
}
#endif