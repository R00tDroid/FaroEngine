#pragma once
#include "ManifestInterface.hpp"

struct FaroProjectsExports Configuration
{
    virtual const char* name() = 0;
    virtual const char* platform() = 0;
    virtual const char* architecture() = 0;
};
