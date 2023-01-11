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

        Path operator+ (String);

    private:
        String path;

        void format_();
    };
}