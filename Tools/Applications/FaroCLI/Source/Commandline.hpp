#pragma once
#include <Parameters.hpp>
#include <Tasks/ITask.hpp>

class ITask;

extern void PrintHelp();
extern bool ParseParameters(ParameterList& parameters, std::vector<ITask*>& tasks, TaskRunInfo& runInfo);
