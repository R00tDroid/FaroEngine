#pragma once
#include "ModuleInfo.hpp"
#include "Worker.hpp"

class ModuleBuild;

class BuildStepInterface
{
public:
    virtual ~BuildStepInterface() = default;
    BuildStepInterface(ModuleBuild* parent) : parent(parent) {}

    virtual void start() = 0;
    virtual bool end() = 0;

    ModuleBuild* moduleBuild() const { return parent; }

private:
    ModuleBuild* parent = nullptr;
};

class ModuleBuild
{
public:
    ModuleBuild(WorkerPool& pool, const BuildSetup& buildSetup, const Toolchain* toolchain, const ModuleManifest* module);

    WorkerPool& pool;
    const BuildSetup& buildSetup;
    const Toolchain* toolchain;
    const ModuleManifest* module;

    void update();
    bool isDone();

    std::mutex sourcesToCompileLock;
    std::set<std::filesystem::path> sourcesToCompile;

private:
    void startStep();

    std::vector<BuildStepInterface*> buildSteps;
    BuildStepInterface* buildStep = nullptr;
    bool shouldContinue = true;
};
