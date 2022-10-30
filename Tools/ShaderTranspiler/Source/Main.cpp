#include <Utility.hpp>
#include <Parameters.hpp>
#include "Version.generated.hpp"
#include "WindowsKitInfo.hpp"

void PrintHelp()
{
    Utility::PrintLine("Faro Engine shader transpiler " + std::string(EngineVersion));
    Utility::Print("-shader <shader paths> Path to one or more shader file to compile\n");
    Utility::Print("-include <directory paths> Path to one or more directories for include scanning\n");
}

int main(int argc, char** argv)
{
    ParameterList parameters(argc, argv);

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    std::filesystem::path dxcExe;
    const std::vector<WindowsKit>& windowsKits = GetWindowsKits();
    for (const WindowsKit& windowsKit : windowsKits)
    {
        std::filesystem::path path = std::filesystem::path(windowsKit.Root) / "bin" / windowsKit.Version / "x64" / "dxc.exe";
        if (std::filesystem::exists(path))
        {
            dxcExe = path;
            break;
        }
    }

    if (dxcExe.empty())
    {
        Utility::PrintLine("Unable to find dxc.exe");
        return -1;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
