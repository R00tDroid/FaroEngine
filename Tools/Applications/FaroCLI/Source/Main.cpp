#include "Parameters.hpp"
#include "Utility.hpp"
#include "Commandline.hpp"

int main(int argc, char** argv)
{
    ParameterList parameters(argc, const_cast<const char**>(argv));

    if (parameters.Contains("help"))
    {
        printHelp();
        return 0;
    }

    TaskInfo taskInfo;

    if (!parseParameters(parameters, taskInfo))
    {
        return -1;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
