#include "Resources.generated.hpp"
#include "sciter-x.h"
#include "Window.hpp"
#include "Console.hpp"
#include <shellapi.h>

int uimain(std::function<int()> run)
{
    sciter::archive::instance().open(aux::elements_of(resources));

    sciter::om::hasset<AppWindow> window = new AppWindow();

    window->load(WSTR("this://app/index.html"));

    window->expand();

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
