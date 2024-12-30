#pragma once
#include <GeneratorInterface.hpp>

class VisualStudioGenerator : public GeneratorInterface
{
public:
    bool generate(const ProjectManifest* project) override;
};
