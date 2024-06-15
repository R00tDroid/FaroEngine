#pragma once
#include "BuilderDll.h"

enum BuildType
{
    Debug,
    Development,
    Release,

    ENUMSIZE
};

struct BuilderExport BuildStageConfiguration
{
    BuildType buildType = BuildType::ENUMSIZE;
    const char* buildPlatform = nullptr;
};

struct BuilderExport BuilderConfiguration
{
    bool runGeneration = false;
    bool runBuild = false;

    BuildStageConfiguration build;

    const char* projectManifestPath = nullptr;
    const char** modules = nullptr;
    unsigned int modulesCount = 0;
};

extern bool performBuildStages(BuilderConfiguration* configuration);
