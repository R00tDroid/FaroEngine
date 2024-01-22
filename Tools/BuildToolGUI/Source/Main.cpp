#include "Resources.generated.hpp"
#include "Window.hpp"
#include "Console.hpp"

int main(int, char*[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) 
    {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return 1;
    }

    int mouseX, mouseY;
    SDL_GetGlobalMouseState(&mouseX, &mouseY);

    int windowSizeX = 430, windowSizeY = 713;
    int windowX = 0, windowY = 0;

    // Move window to monitor in use by user
    int monitorCount = SDL_GetNumVideoDisplays();
    for (int i = 0; i < monitorCount; i++)
    {
        SDL_Rect monitorRect;
        if (SDL_GetDisplayBounds(i, &monitorRect) == 0)
        {
            if (mouseX >= monitorRect.x && mouseX <= monitorRect.x + monitorRect.w &&
                mouseY >= monitorRect.y && mouseY <= monitorRect.y + monitorRect.h)
            {
                windowX = monitorRect.x + monitorRect.w / 2 - windowSizeX / 2;
                windowY = monitorRect.y + monitorRect.h / 2 - windowSizeY / 2;
                break;
            }
        }
    }

    AppWindow* window = new AppWindow({ windowX, windowY, windowSizeX, windowSizeY });

    // Parse commandline arguments
    /*std::wstring commandLine = GetCommandLineW();
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(commandLine.c_str(), &argc);
    if (argc > 1)
    {
        std::filesystem::path projectPath = argv[1];
        projectPath = std::filesystem::absolute(projectPath);
        projectPath = std::filesystem::weakly_canonical(projectPath);

        window->SetProjectPath(projectPath.wstring());
    }*/

    Console::Init();

    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    delete window;
                    return 0;
                }
            }
        }

        window->Render();
    }
}
