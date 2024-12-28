#pragma once
#include "ManifestInterface.hpp"

struct FaroProjectsExports Configuration
{
    virtual const char* name() const = 0;
    virtual const char* platform() const = 0;
    virtual const char* architecture() const = 0;
};
