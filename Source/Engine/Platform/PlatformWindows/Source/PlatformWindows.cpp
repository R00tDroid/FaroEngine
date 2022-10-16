#include "PlatformWindows.hpp"
#include <iostream>
#include <Log.hpp>
#include <MinWindows.hpp>
#include <WindowWindows.hpp>
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    String FormatLogString(LogTag& tag, ELogCategory& category, String& message)
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

        return FormatString("[%-15s](%s) %s", tag.Data(), categoryLabel.Data(), message.Data());
    }

    void LogCout(LogTag tag, ELogCategory category, String message)
    {
        String string = FormatLogString(tag, category, message);
        std::cout << string.Data() << std::endl;
    }

    void LogDebugOutput(LogTag tag, ELogCategory category, String message)
    {
        String string = FormatLogString(tag, category, message);
        OutputDebugStringA((string + "\n").Data());
    }

    void PlatformWindows::Init()
    {
        AddLogSink(LogCout);
        AddLogSink(LogDebugOutput);
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
