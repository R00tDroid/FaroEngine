#include "Path.h"
#include <algorithm>
#include "../Containers/Array.h"
#include <sys/stat.h>

Faro::Path::Path()
{
    path = "";
}

Faro::Path::Path(String path)
{
    path = path;
    format_();
}

Faro::Path Faro::Path::ParentDirectory()
{
    int32 pos = path.FindLastOf("\\");

    if (pos != -1) 
    {
        String p = path.Sub(pos);
        return Path(p);
    }
    else
    {
        return Path("");
    }
}

bool Faro::Path::Exists()
{
    struct stat info;
    return stat(*path, &info) == 0;
}

Faro::PathType Faro::Path::GetType()
{
    struct stat info;
    if (stat(*path, &info) == 0) 
    {
        if (info.st_mode & S_IFDIR)
        {
            return Directory;
        }
        else if (info.st_mode & S_IFREG)
        {
            return Filepath;
        }
    }

    return Unknown;
}

Faro::String Faro::Path::Get()
{
    return path;
}

Faro::Path Faro::Path::GetFileName()
{
    int32 Index = path.FindLastOf("\\");
    if (Index == -1)
        return Path(path);
    else
        return Path(path.Sub(Index + 1, path.Length() - Index));
}

Faro::Path Faro::Path::RemoveExtension()
{
    int32 Index = path.FindLastOf(".");
    if (Index == -1)
        return Path(path);
    else
        return Path(path.Sub(Index));
}

Faro::Path Faro::Path::operator+(String Extension)
{
    return Path(path + Extension);
}

void Faro::Path::format_()
{
    std::replace(path.begin(), path.end(), '/', '\\');
    path.Replace("\\\\", "\\");

    if (path.Find("..") != -1)
    {
        Array<String> sections;
        String s = path;
        int16 pos = 0;
        String token;

        while ((pos = s.Find("\\")) != -1)
        {
            sections.Add(s.Sub(0, pos));
            s.Erase(0, pos + 1);
        }
        sections.Add(s);

        for (size_t i = 0; i < sections.Size(); i++)
        {
            if (sections[i] == ".." && i > 0)
            {
                if (!sections[i - 1].Equals(".."))
                {
                    sections.RemoveAt(i);
                    sections.RemoveAt(i - 1);
                    i--;
                }
            }
        }

        path = "";
        for (size_t i = 0; i < sections.Size(); i++)
        {
            path += sections[i];

            if (i < sections.Size() - 1)
            {
                path += "\\";
            }
        }
    }
}
