#include <Utility.hpp>
#include <Parameters.hpp>
#include "Version.generated.hpp"

void PrintHelp()
{
    Utility::PrintLine("Faro Engine shader transpiler " + std::string(EngineVersion));
}

int main(int argc, char** argv)
{
    ParameterList parameters(argc, argv);

    Utility::PrintLine("No tasks specified");
    return -1;
}
