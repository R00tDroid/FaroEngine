#include "PlatformWindows.hpp"
#include <iostream>
#include <Log.hpp>
#include <MinWindows.hpp>
#include <WindowWindows.hpp>
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    String FormatLogString(const LogTag& tag, ELogCategory& category, const String& message)
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

    void LogCout(const LogTag& tag, ELogCategory category, const String& message)
    {
        String string = FormatLogString(tag, category, message);
        std::cout << string.Data() << std::endl;
    }

    void LogDebugOutput(const LogTag& tag, ELogCategory category, const String& message)
    {
        String string = FormatLogString(tag, category, message);
        OutputDebugStringA((string + "\n").Data());
    }

    void PlatformWindows::Init()
    {
        Logger::AddSink(LogCout);
        Logger::AddSink(LogDebugOutput);
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
}
