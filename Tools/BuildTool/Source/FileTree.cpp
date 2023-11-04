#include "FileTree.hpp"
#include "Manifests/ModuleManifest.hpp"
#include <fstream>
#include <Utility.hpp>

FileTree::FileTree(ModuleManifest* inModule) : module(inModule)
{
}

std::set<std::filesystem::path> BuildIncludeTree(const std::filesystem::path& file, std::map<std::filesystem::path, std::set<std::filesystem::path>>& resolvedIncludes)
{
    std::set<std::filesystem::path> includes;

    auto it = resolvedIncludes.find(file);
    if (it != resolvedIncludes.end())
    {
        includes = it->second;
    }

    for (const std::filesystem::path& includeFile : includes)
    {
        std::set<std::filesystem::path> treeInclude = BuildIncludeTree(includeFile, resolvedIncludes);
        for (const std::filesystem::path& include : treeInclude)
        {
            includes.insert(include);
        }
    }

    return includes;
}

void FileTree::Parse()
{
    std::set<ModuleManifest*> modules = module->GetDependencyTree();
    modules.insert(module);

    std::set<std::filesystem::path> filePaths;
    for (const ModuleManifest* parsingModule : modules)
    {
        for (const std::filesystem::path& filePath : parsingModule->sourceFiles)
        {
            filePaths.insert(filePath);
        }
    }

    std::map<std::filesystem::path, std::set<std::filesystem::path>> resolvedIncludes;

    for (const ModuleManifest* parsingModule : modules)
    {
        std::vector<std::filesystem::path> includeDirectories = parsingModule->GetModuleIncludeDirectories();

        for (const std::filesystem::path& filePath : parsingModule->sourceFiles)
        {
            std::set<std::filesystem::path> resolvedFiles;

            std::set<std::string> relativeIncludes;
            std::set<std::string> absoluteIncludes;

            if (FindIncludes(filePath, relativeIncludes, absoluteIncludes))
            {
                for (std::filesystem::path relativeFile : relativeIncludes)
                {
                    std::filesystem::path path = filePath.parent_path() / relativeFile;
                    path = std::filesystem::weakly_canonical(path);

                    auto it = filePaths.find(path);
                    if (it != filePaths.end())
                    {
                        resolvedFiles.insert(path);
                    }
                }

                for (std::filesystem::path absoluteFile : absoluteIncludes)
                {
                    for (const std::filesystem::path& includeDirectory : includeDirectories)
                    {
                        std::filesystem::path path = includeDirectory / absoluteFile;
                        path = std::filesystem::weakly_canonical(path);

                        auto it = filePaths.find(path);
                        if (it != filePaths.end())
                        {
                            resolvedFiles.insert(path);
                            break;
                        }
                    }
                }
            }
            else
            {
                Utility::PrintLine("Failed to parse includes for: " + filePath.string());
            }

            resolvedIncludes.insert(std::pair<std::filesystem::path, std::set<std::filesystem::path>>(filePath, resolvedFiles));
        }
    }

    for (const std::filesystem::path& filePath : module->sourceFiles)
    {
        includedFiles.insert(std::pair<std::filesystem::path, std::set<std::filesystem::path>>(filePath, BuildIncludeTree(filePath, resolvedIncludes)));
    }
}

std::set<std::filesystem::path> FileTree::GetTree(const std::filesystem::path& file)
{
    auto it = includedFiles.find(file);
    if (it != includedFiles.end())
    {
        return it->second;
    }

    return {};
}

bool FileTree::FindIncludes(const std::filesystem::path& file, std::set<std::string>& relativeIncludes, std::set<std::string>& absoluteIncludes)
{
    std::ifstream stream(file);
    if (!stream.is_open()) return false;

    std::string line;
    while (std::getline(stream, line)) //TODO Ignore commented lines
    {
        if (line.find("#include") == std::string::npos) continue;

        std::vector<std::string> matches;
        if (Utility::MatchPattern(line, ".*#include.*\"(.*)\".*", matches))
        {
            relativeIncludes.insert(matches[0]);
        }
        else if (Utility::MatchPattern(line, ".*#include.*<(.*)>.*", matches))
        {
            absoluteIncludes.insert(matches[0]);
        }
    }

    return true;
}
