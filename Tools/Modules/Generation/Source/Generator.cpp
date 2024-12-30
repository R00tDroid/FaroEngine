#include "Generator.hpp"
#include <vector>
#include "GeneratorInterface.hpp"
#include "Generators/VisualStudioGenerator.hpp"

std::vector<GeneratorInterface*> generators = { new VisualStudioGenerator()};

bool Generator::generate(const ProjectManifest* project)
{
    bool success = true;
    for (GeneratorInterface* generator : generators)
    {
        if (!generator->generate(project)) success = false;
    }

    return success;
}
