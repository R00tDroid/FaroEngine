#include <Utility.hpp>
#include <Parameters.hpp>
#include "Command.hpp"
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

    std::vector<std::filesystem::path> shaderFiles;
    if (parameters.Contains("shader"))
    {
        if (parameters.CountArguments("shader") == 0)
        {
            Utility::PrintLine("-shader requires at least one path");
            return -1;
        }

        bool anyFailed = false;
        for (std::string shaderPathString : parameters.GetArguments("shader"))
        {
            std::filesystem::path shaderPath = std::filesystem::absolute(shaderPathString);
            shaderPath = weakly_canonical(shaderPath);
            shaderPath.make_preferred();

            if (!std::filesystem::exists(shaderPath))
            {
                Utility::PrintLine("Could not find input file: " + shaderPath.string());
                anyFailed = true;
                continue;
            }

            shaderFiles.push_back(shaderPath);
        }

        if (anyFailed) return -1;
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

    if (!shaderFiles.empty())
    {
        for (std::filesystem::path& shaderFile : shaderFiles)
        {
            std::string command = "\"" + dxcExe.string() + "\" \"" + shaderFile.string() + "\"";
            std::string log;
            int result = ExecuteCommand(command, log);

            if (!log.empty())
            {
                Utility::Print(log);
            }

            if (result != 0)
            {
                Utility::PrintLine("Failed to compile shader");
                return -1;
            }
        }

        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
