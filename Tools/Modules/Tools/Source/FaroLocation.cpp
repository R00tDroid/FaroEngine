#include "FaroLocation.hpp"
#include <filesystem>
#ifdef WIN32
#include <Windows.h>
#endif

struct Locations
{
    std::string toolsBin;
    std::string buildTool;
};
Locations locations;
bool gotLocations = false;

void getLocations()
{
    if (!gotLocations)
    {
        gotLocations = true;
        std::filesystem::path toolsBin;

#ifdef WIN32
        char filename[MAX_PATH];
        GetModuleFileNameA(nullptr, filename, MAX_PATH);
        toolsBin = { filename };
#else
        toolsBin = { std::filesystem::canonical("/proc/self/exe") };
#endif

        toolsBin = toolsBin.parent_path();
        toolsBin.make_preferred();
        toolsBin = weakly_canonical(toolsBin);

        std::filesystem::path buildTool = toolsBin / "FaroBuild";

        locations.toolsBin = toolsBin.string();
        locations.buildTool = buildTool.string();
    }
}

const char* FaroLocation::toolsBin()
{
    getLocations();
    return locations.toolsBin.c_str();
}

const char* FaroLocation::buildTool()
{
    getLocations();
    return locations.buildTool.c_str();
}
