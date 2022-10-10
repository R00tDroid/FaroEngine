#include "Resources.generated.hpp"
#include "sciter-x.h"
#include "Window.hpp"
#include "Console.hpp"
#include <shellapi.h>

int uimain(std::function<int()> run)
{
    sciter::archive::instance().open(aux::elements_of(resources));

    int windowSizeX = 430, windowSizeY = 713;

    // Get monitor from cursor position
    POINT cursorPosition;
    GetCursorPos(&cursorPosition);
    HMONITOR userMonitor = MonitorFromPoint(cursorPosition, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoA(userMonitor, &monitorInfo);

    // Move window to monitor in use by user
    int monitorCenterX = (monitorInfo.rcMonitor.left + monitorInfo.rcMonitor.right) / 2;
    int monitorCenterY = (monitorInfo.rcMonitor.top + monitorInfo.rcMonitor.bottom) / 2;
    int windowX = monitorCenterX - windowSizeX / 2;
    int windowY = monitorCenterY - windowSizeY / 2;

    sciter::om::hasset<AppWindow> window = new AppWindow({ windowX, windowY, windowX + windowSizeX, windowY + windowSizeY });

    // Setup GUI
    window->load(WSTR("this://app/index.html"));
    window->expand();

    // Parse commandline arguments
    std::wstring commandLine = GetCommandLineW();
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(commandLine.c_str(), &argc);
    if (argc > 1)
    {
        std::filesystem::path projectPath = argv[1];
        projectPath = std::filesystem::absolute(projectPath);
        projectPath = std::filesystem::weakly_canonical(projectPath);

        BEHAVIOR_EVENT_PARAMS params = { 0 };
        params.name = WSTR("setProjectPath");
        params.data = projectPath.string();
        AppWindow::broadcast_event(params);
    }

    Console::Init();

    return run();
}
