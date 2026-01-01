#include "ChangeDatabase.hpp"

void ChangeDB::save(const std::set<std::filesystem::path>&) const
{
    //TODO Save DB
}

bool ChangeDB::load()
{
    //TODO Load DB
    return false;
}

bool ChangeDB::hasChanged(const std::filesystem::path& file) const
{
    const auto& it = dbTimes.find(file);
    if (it == dbTimes.end()) return true;
    const ChangeDBTime& dbTime = it->second;

    ChangeDBTime currentTime = getFileTime(file);
    return dbTime != currentTime;
}

bool ChangeDB::anyChanges()
{
    for (const auto& it : dbTimes)
    {
        const std::filesystem::path& file = it.first;
        const ChangeDBTime& dbTime = it.second;

        if (!std::filesystem::exists(file)) return true;

        ChangeDBTime currentTime = getFileTime(file);
        if (dbTime != currentTime) return true;
    }
    return false;
}

ChangeDBTime ChangeDB::getFileTime(const std::filesystem::path& file) const
{
    std::filesystem::file_time_type currentTime = std::filesystem::last_write_time(file);
    return currentTime.time_since_epoch().count();
}
