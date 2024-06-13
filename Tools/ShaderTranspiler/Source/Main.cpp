#include <Utility.hpp>
#include <Parameters.hpp>
#include "Command.hpp"
#include <ShaderDefinition.hpp>
#include "ShaderCompiler.hpp"
#include "ShaderPacker.hpp"

#ifdef _WIN32
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#endif

void PrintHelp()
{
    Utility::PrintLine("Faro Engine shader transpiler");
    Utility::Print("-shader <shader paths> Path to one or more shader file to compile\n");
    Utility::Print("-include <directory paths> Path to one or more directories for include scanning\n");
}

std::vector<std::filesystem::path> shaderFiles;
std::vector<std::filesystem::path> includeDirectories;

bool ParseParameters(ParameterList& parameters)
{
    if (parameters.Contains("shader"))
    {
        if (parameters.CountArguments("shader") == 0)
        {
            Utility::PrintLine("-shader requires at least one path");
            return false;
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

        if (anyFailed) return false;
    }

    if (parameters.Contains("include"))
    {
        if (parameters.CountArguments("include") == 0)
        {
            Utility::PrintLine("-include requires at least one path");
            return false;
        }

        bool anyFailed = false;
        for (std::string pathString : parameters.GetArguments("include"))
        {
            std::filesystem::path path = std::filesystem::absolute(pathString);
            path = weakly_canonical(path);
            path.make_preferred();

            includeDirectories.push_back(path);
        }

        if (anyFailed) return false;
    }

    return true;
}

bool PackageShader(std::filesystem::path& sourcePath, Shader& shader)
{
    std::filesystem::path packagePath = sourcePath;
    packagePath.replace_extension(".g.shader");

    ShaderPacker packer;
    packer.Pack(packagePath, shader);

    return true;
}

bool CompileShaders()
{
    ShaderCompiler compiler;
    compiler.SetSearchDirectory(includeDirectories);

    for (std::filesystem::path& shaderFile : shaderFiles)
    {
        Shader shader;

        Utility::PrintLine(shaderFile.filename().string());

        std::string outputPath = (shaderFile.parent_path() / shaderFile.stem()).string();

        if (
            !compiler.CompileShader(shaderFile, shader.dxil.vertex, ShaderCompiler::CF_DXIL, ShaderCompiler::SS_Vertex) ||
            !compiler.CompileShader(shaderFile, shader.dxil.pixel, ShaderCompiler::CF_DXIL, ShaderCompiler::SS_Pixel)
            )
        {
            Utility::PrintLine("Failed to compile shader");
            return false;
        }

        if (
            !compiler.CompileShader(shaderFile, shader.spirV.vertex, ShaderCompiler::CF_SpirV, ShaderCompiler::SS_Vertex) ||
            !compiler.CompileShader(shaderFile, shader.spirV.pixel, ShaderCompiler::CF_SpirV, ShaderCompiler::SS_Pixel)
            )
        {
            Utility::PrintLine("Failed to compile shader");
            return false;
        }

        if (!PackageShader(shaderFile, shader))
        {
            Utility::PrintLine("Failed to package shader");
            return false;
        }
    }

    return true;
}

int main_function(int argc, char** argv) //TODO Remove main
{
    ParameterList parameters(argc, const_cast<const char**>(argv));

    if (parameters.Contains("help"))
    {
        PrintHelp();
        return 0;
    }

    if (!ParseParameters(parameters)) return -1;

    if (!shaderFiles.empty())
    {
        if (!CompileShaders()) return -1;
        return 0;
    }

    Utility::PrintLine("No tasks specified");
    return -1;
}
