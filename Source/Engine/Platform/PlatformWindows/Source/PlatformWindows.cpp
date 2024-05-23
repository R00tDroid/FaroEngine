#if FARO_PLATFORM_WINDOWS
#include "PlatformWindows.hpp"
#include <iostream>
#include <Log.hpp>
#include <MinWindows.hpp>
#include <WindowWindows.hpp>
#include <Memory/MemoryManager.hpp>
#include <FileStreamWindows.hpp>

namespace Faro
{
    String FormatLogString(const LogTag& tag, const LogCategory& category, const String& message)
    {
        String categoryLabel = "";

        switch (category)
        {
            case LC_Trace: { categoryLabel = "T"; break; }
            case LC_Debug: { categoryLabel = "D"; break; }
            case LC_Info: { categoryLabel = "I"; break; }
            case LC_Warning: { categoryLabel = "W"; break; }
            case LC_Error: { categoryLabel = "E"; break; }
            case LC_Fatal: { categoryLabel = "F"; break; }
        }

        return FormatString("[%-15s](%s) %s", tag.name.Data(), categoryLabel.Data(), message.Data());
    }

    class LogCout : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override
        {
            String string = FormatLogString(message.tag, message.category, message.message);
            std::cout << string.Data() << std::endl;
        }
    };
    REGISTER_LOGSINK(LogCout)

    class LogDebugOutput : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override
        {
            String string = FormatLogString(message.tag, message.category, message.message);
            OutputDebugStringA((string + "\n").Data());
        }
    };
    REGISTER_LOGSINK(LogDebugOutput)

    void PlatformWindows::Init()
    {
    }

    void PlatformWindows::Destroy()
    {
    }

    Window* PlatformWindows::CreateWindow()
    {
        Window* window = MemoryManager::New<WindowWindows>();
        window->Init();
        return window;
    }

    BOOL QueryMonitors(HMONITOR monitorHandle, HDC deviceContext, LPRECT rect, LPARAM userData)
    {
        Array<Monitor>& monitors = *reinterpret_cast<Array<Monitor>*>(userData);

        Monitor monitor;
        monitor.desktop = IntRect(rect->left, rect->top, rect->right - rect->left, rect->bottom - rect->top);

        MONITORINFOEX monitorInfo;
        MemoryManager::Zero(&monitorInfo);

        monitorInfo.cbSize = sizeof(monitorInfo);
        if (SUCCEEDED(GetMonitorInfoA(monitorHandle, &monitorInfo)))
        {
            monitor.identifier = monitorInfo.szDevice;
        }

        monitors.Add(monitor);

        return TRUE;
    }

    Array<Monitor> PlatformWindows::GetMonitors()
    {
        Array<Monitor> monitors;
        EnumDisplayMonitors(nullptr, nullptr, QueryMonitors, reinterpret_cast<LPARAM>(&monitors));
        return monitors;
    }

    FileStream* PlatformWindows::OpenFile(Path file, EFileMode mode)
    {
        FileStreamWindows* stream = MemoryManager::New<FileStreamWindows>();
        stream->Init(file, mode);
        return stream;
    }

    Array<Path> PlatformWindows::FindSubPaths(Path rootPath, bool recursive, String filter)
    {
        Array<Path> paths;

        WIN32_FIND_DATAA searchInfo;
        HANDLE searchOperation = FindFirstFileA((rootPath + filter).Get().Data(), &searchInfo);

        while (searchOperation != INVALID_HANDLE_VALUE)
        {
            if (strcmp(searchInfo.cFileName, ".") != 0 && strcmp(searchInfo.cFileName, "..") != 0 && strlen(searchInfo.cFileName) > 0)
            {
                Path path = rootPath + searchInfo.cFileName;
                paths.Add(path);

                if (recursive && path.GetType() == PathType::Directory)
                {
                    Array<Path> subPaths = FindSubPaths(path, recursive, filter);
                    paths.Add(subPaths);
                }
            }

            if (!FindNextFileA(searchOperation, &searchInfo))
            {
                break;
            }
        }
        FindClose(searchOperation);

        return paths;
    }
}
#endif
