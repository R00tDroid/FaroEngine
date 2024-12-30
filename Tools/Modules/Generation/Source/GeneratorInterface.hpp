#pragma once
#include <ProjectInfo.hpp>

class GeneratorInterface
{
public:
    virtual ~GeneratorInterface() = default;
    virtual bool generate(ProjectManifest* project) = 0;
};
