#include "Path.h"
#include <algorithm>
#include "../Containers/Array.h"
#include <sys/stat.h>

Faro::Path::Path()
{
	path_ = "";
}

Faro::Path::Path(String path)
{
	path_ = path;
	format_();
}

Faro::Path Faro::Path::ParentDirectory()
{
	int32 pos = path_.FindLastOf("\\");

	if (pos != -1) 
	{
		String p = path_.Sub(pos);
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
	return stat(*path_, &info) == 0;
}

Faro::PathType Faro::Path::GetType()
{
	struct stat info;
	if (stat(*path_, &info) == 0) 
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
	return path_;
}

Faro::Path Faro::Path::GetFileName()
{
	int32 Index = path_.FindLastOf("\\");
	if (Index == -1)
		return Path(path_);
	else
		return Path(path_.Sub(Index + 1, path_.Length() - Index));
}

Faro::Path Faro::Path::RemoveExtension()
{
	int32 Index = path_.FindLastOf(".");
	if (Index == -1)
		return Path(path_);
	else
		return Path(path_.Sub(Index));
}

Faro::Path Faro::Path::operator+(String Extension)
{
	return Path(path_ + Extension);
}

void Faro::Path::format_()
{
	std::replace(path_.begin(), path_.end(), '/', '\\');
	path_.Replace("\\\\", "\\");

	if (path_.Find("..") != -1)
	{
		Array<String> sections;
		String s = path_;
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

		path_ = "";
		for (size_t i = 0; i < sections.Size(); i++)
		{
			path_ += sections[i];

			if (i < sections.Size() - 1)
			{
				path_ += "\\";
			}
		}
	}

}
