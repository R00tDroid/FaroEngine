#pragma once
#include <filesystem>

struct FileTimeInfo
{
    std::filesystem::path file;
    std::vector<FileTimeInfo*> includeChildren;
    std::vector<FileTimeInfo*> includeParents;
};

class ModuleFileDates
{
public:

};
