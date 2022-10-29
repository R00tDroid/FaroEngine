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

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
