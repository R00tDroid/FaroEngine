#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"
#include <string>
#include <string_view>
#include "FileParser.hpp"

bool Reflector::generateFileReflection(const ModuleManifest* moduleManifest, unsigned int fileIndex)
{
    std::filesystem::path file = moduleManifest->sourceFile(fileIndex);
    std::ifstream fileStream(file);

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    std::string fileBuffer(size, ' ');
    fileStream.seekg(0);
    fileStream.read(fileBuffer.data(), size);

    std::vector<ReflectedEntry*> entries;
    if (!fileReflector.reflect(fileBuffer, entries))
    {
        return false;
    }

    if (!entries.empty())
    {
        //TODO Process reflected types

        for (const ReflectedEntry* entry : entries)
        {
            Utility::PrintLineD("Reflected " + entry->name);
            delete entry;
        }
    }

    return true;
}

bool Reflector::generateModuleReflection(const ModuleManifest*)
{
    return true;
}
