#include "ChangeDatabase.hpp"
#include "Utility.hpp"
#include <fstream>

typedef unsigned int ChangeDBLength;

ChangeDB::ChangeDB(const std::filesystem::path&& database) : database(database) {}

void ChangeDB::save(const std::set<std::filesystem::path>& files) const
{
    std::filesystem::path directory = database.parent_path();
    Utility::EnsureDirectory(directory.string().c_str());

    std::ofstream stream(database, std::ios::binary);
    if (!stream.is_open())
    {
        Utility::PrintLine("Failed to save change database");
        return;
    }

    for (const std::filesystem::path& file : files)
    {
        std::string path = file.string();
        ChangeDBLength pathLength = static_cast<ChangeDBLength>(path.size());
        stream.write(reinterpret_cast<const char*>(&pathLength), sizeof(ChangeDBLength));
        stream.write(path.c_str(), pathLength);

        ChangeDBTime fileTime = getFileTime(file);
        stream.write(reinterpret_cast<const char*>(&fileTime), sizeof(ChangeDBTime));
    }

    stream.close();
}

bool ChangeDB::load()
{
    dbTimes.clear();

    if (!std::filesystem::exists(database))
    {
        Utility::PrintLineD("Missing change database");
        return false;
    }

    std::ifstream stream(database, std::ios::binary);
    if (!stream.is_open())
    {
        Utility::PrintLine("Failed to read change database");
        return false;
    }

    while (!stream.eof()) 
    {
        //TODO Add safety checks to ensure enough data is available
        ChangeDBLength length = 0;
        stream.read(reinterpret_cast<char*>(&length), sizeof(ChangeDBLength));
        std::string pathString;
        pathString.resize(length);
        stream.read(pathString.data(), length);

        std::filesystem::path path(pathString);

        ChangeDBTime timestamp = 0;
        stream.read(reinterpret_cast<char*>(&timestamp), sizeof(ChangeDBTime));

        dbTimes.insert(std::pair(path, timestamp));
    }

    stream.close();

    return true;
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
