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

std::filesystem::path dxcExe;

bool CompileShader(std::filesystem::path& file, std::filesystem::path output, std::string parameters)
{
    std::filesystem::path dxcDrive = dxcExe.root_name();
    std::string command = dxcDrive.string() + " & \"" + dxcExe.string() + "\" -Qstrip_debug -Qstrip_reflect -Fo \"" + output.string() + "\" " + parameters + " \"" + file.string() + "\"";
    std::string log;
    int result = Utility::ExecuteCommand(command, log);

    if (!log.empty())
    {
        Utility::Print(log);
    }

    return result == 0;
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
            std::string outputPath = (shaderFile.parent_path() / shaderFile.stem()).string();

            if (
                !CompileShader(shaderFile, outputPath + ".vs.dxbc", "-T vs_6_2 -E VSMain") ||
                !CompileShader(shaderFile, outputPath + ".ps.dxbc", "-T ps_6_2 -E PSMain")
                )
            {
                Utility::PrintLine("Failed to compile shader");
                return -1;
            }

            if (
                !CompileShader(shaderFile, outputPath + ".vs.spv", "-T vs_6_2 -E VSMain -spirv") ||
                !CompileShader(shaderFile, outputPath + ".ps.spv", "-T ps_6_2 -E PSMain -spirv")
                )
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
