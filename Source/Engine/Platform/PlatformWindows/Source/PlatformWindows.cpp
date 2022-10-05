#include "PlatformWindows.hpp"
#include <iostream>
#include <Log.hpp>
#include <MinWindows.hpp>
#include <WindowWindows.hpp>
#include <Memory/MemoryManager.hpp>

namespace Faro
{
    void LogWindows(LogTag tag, ELogCategory category, String message)
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

        String result = FormatString("[%-15s](%s) %s", tag.Data(), categoryLabel.Data(), message.Data());
        std::cout << result.Data() << std::endl;
        OutputDebugStringA((result + "\n").Data());
    }

    void PlatformWindows::Init()
    {
        SetLogSink(LogWindows);
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

        MONITORINFOEX MonitorInfo;
        MemoryManager::Zero(&MonitorInfo);

        MonitorInfo.cbSize = sizeof(MonitorInfo);
        if (SUCCEEDED(GetMonitorInfoA(monitorHandle, &MonitorInfo)))
        {
            monitor.identifier = MonitorInfo.szDevice;
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
