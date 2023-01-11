#pragma once
#include "../Containers/String.hpp"

namespace Faro
{
    enum PathType
    {
        Unknown,
        Filepath,
        Directory
    };

    class Path
    {
    public:
        Path(const Path& path);
        Path();
        Path(String path);

        Path ParentDirectory();

        bool Exists();

        PathType GetType();

        String Get();

        Path GetFileName();

        Path RemoveExtension();

        Path GetRelative(Path path) const;

        Path operator+(String);

        bool operator <(const Path& other) const
        {
            return path < other.path;
        }

    private:
        String path;

        void format_();
    };
}