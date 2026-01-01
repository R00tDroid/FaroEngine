#pragma once
#include <filesystem>
#include <set>
#include <map>

typedef long long ChangeDBTime;

class ChangeDB
{
public:
    ChangeDB(const std::filesystem::path&& database);

    void save(const std::set<std::filesystem::path>& files) const;
    bool load();

    bool hasChanged(const std::filesystem::path& file) const;
    bool anyChanges();

private:
    ChangeDBTime getFileTime(const std::filesystem::path& file) const;

    std::filesystem::path database;
    std::map<std::filesystem::path, ChangeDBTime> dbTimes;
};
