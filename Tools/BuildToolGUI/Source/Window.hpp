#pragma once
#include <string>
#include <SDL2/SDL.h>

class AppWindow
{
public:
    AppWindow(SDL_Rect);
    ~AppWindow();

    void SetProjectPath(std::wstring path);

    void Render();

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
};
