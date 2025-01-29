#pragma once
#include <Parameters.hpp>
#include "BuildSetup.hpp"

struct TaskInfo
{
    bool runConfigure = false;
    bool runProject = false;
    bool runBuild = false;
    bool runClean = false;
    bool runDeploy = false;

    std::filesystem::path manifest = {};
    std::vector<std::string> modules = {};
    std::string platformName = "";
    Configuration config = C_ENUMSIZE;
};

extern void printHelp();
extern bool parseParameters(ParameterList& parameters, TaskInfo& taskInfo);
