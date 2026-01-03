#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"

bool Reflector::generateFileReflection(const ModuleManifest* moduleManifest, unsigned int fileIndex)
{
    std::filesystem::path file = moduleManifest->sourceFile(fileIndex);

    std::ifstream fileStream(file);

    fileStream.seekg(0, std::ios::end);
    size_t size = fileStream.tellg();
    std::string fileBuffer(size, ' ');
    fileStream.seekg(0);
    fileStream.read(fileBuffer.data(), size);

    while (true)
    {
        size_t classStart = fileBuffer.find(" Class(");
        if (classStart == std::string::npos) break;

        fileBuffer.erase(fileBuffer.begin(), fileBuffer.begin() + classStart);

        Utility::PrintLine("");
    }

    return true;
}

bool Reflector::generateModuleReflection(const ModuleManifest*)
{
    return true;
}
