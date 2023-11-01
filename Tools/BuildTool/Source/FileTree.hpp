#pragma once
#include <filesystem>
#include <map>
#include <vector>

class ModuleManifest;

class FileTreeGenerator
{
public:
    static void ParseTree(std::vector<ModuleManifest*> modules);
};
