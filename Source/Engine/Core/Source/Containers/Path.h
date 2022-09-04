#pragma once
#include "../Containers/String.h"

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