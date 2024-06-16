#include <vector>
#include "Parameters.hpp"
#include "Utility.hpp"
#include "Commandline.hpp"
#include "BuildConfig.hpp"

int main(int argc, char** argv)
{
    PerformanceTimer::StartGlobalTimer();

    ParameterList parameters(argc, const_cast<const char**>(argv));

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    BuilderConfiguration buildConfig;

    if (!ParseParameters(parameters, buildConfig))
    {
        return -1;
    }

    if (performBuildStages(&buildConfig))
    {
        return 0;
    }

    return -1;
}
