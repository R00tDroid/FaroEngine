#include "Platform.hpp"

namespace Faro
{
    Platform* GPlatform = nullptr;

    Monitor Platform::GetPrimaryMonitor()
    {
        for (Monitor& monitor : GetMonitors())
        {
            if (monitor.desktop.topLeft == Int2D::zero)
            {
                return monitor;
            }
        }
        return Monitor();
    }

    Array<Path> Platform::FindFiles(Path rootPath, bool recursive, String filter)
    {
        Array<Path> paths = FindSubPaths(rootPath, recursive, filter);
        Array<Path> result;
        for (Path& path : paths)
        {
            if (path.GetType() == PathType::Filepath)
            {
                result.Add(path);
            }
        }
        return result;
    }

    Array<Path> Platform::FindDirectories(Path rootPath, bool recursive, String filter)
    {
        Array<Path> paths = FindSubPaths(rootPath, recursive, filter);
        Array<Path> result;
        for (Path& path : paths)
        {
            if (path.GetType() == PathType::Directory)
            {
                result.Add(path);
            }
        }
        return result;
    }

    Array<Platform*> GetRegisteredPlatforms()
    {
        return {};// InstanceRegistry_Platform::Get().GetValues();
    }
}
