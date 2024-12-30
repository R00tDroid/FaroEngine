#pragma once
#include <GeneratorInterface.hpp>

class VisualStudioGenerator : public GeneratorInterface
{
public:
    bool generate(ProjectManifest* project) override;
};
