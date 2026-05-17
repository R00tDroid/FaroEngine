#include "Reflector.hpp"
#include <filesystem>
#include <fstream>
#include "ModuleInfo.hpp"
#include <string>
#include <string_view>
#include "FileParser.hpp"

static void reflectEntry(const ReflectedEntry* entry, std::ofstream& file);

static void reflectClass(const ReflectedClass* entry, std::ofstream& file)
{
    file << "// Reflected class: " << entry->name << "\n";

    for (auto member : entry->members)
    {
        reflectEntry(member, file);
    }
}

static void reflectStruct(const ReflectedClass* entry, std::ofstream& file)
{
    file << "// Reflected struct: " << entry->name << "\n";

    for (auto member : entry->members)
    {
        reflectEntry(member, file);
    }
}

static void reflectFunction(const ReflectedFunction* entry, std::ofstream& file)
{
    file << "// Reflected function: " << entry->name << "\n";
}

static void reflectVariable(const ReflectedVariable* entry, std::ofstream& file)
{
    file << "// Reflected variable: " << entry->name << "\n";
}


static void reflectEntry(const ReflectedEntry* entry, std::ofstream& file)
{
    switch (entry->type)
    {
	    case RT_Class:
	    {
	        reflectClass(dynamic_cast<const ReflectedClass*>(entry), file);
	        break;
	    }

	    case RT_Struct:
	    {
	        reflectStruct(dynamic_cast<const ReflectedClass*>(entry), file);
	        break;
	    }
	    case RT_Variable:
	    {
	        reflectVariable(dynamic_cast<const ReflectedVariable*>(entry), file);
	        break;
	    }
	    case RT_Function:
	    {
	        reflectFunction(dynamic_cast<const ReflectedFunction*>(entry), file);
	        break;
	    }
    }
}


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
        //TODO Don't overwrite if multiple files have reflected members
        //TODO Add generated file to source for compilation

        std::filesystem::path generatedFolder = moduleManifest->getGeneratedDirectory();
        if (!std::filesystem::exists(generatedFolder))
        {
	        create_directories(generatedFolder);
        }

        std::filesystem::path filePath = generatedFolder / "Reflection.cpp";

        std::ofstream outStream(filePath);

        for (const ReflectedEntry* entry : entries)
        {
            reflectEntry(entry, outStream);

            Utility::PrintLineD("Reflected " + entry->name);
            delete entry;
        }

        outStream.close();
    }

    return true;
}

bool Reflector::generateModuleReflection(const ModuleManifest*)
{
    return true;
}
