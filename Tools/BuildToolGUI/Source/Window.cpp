#include "Window.hpp"
#include "Console.hpp"

#define NK_IMPLEMENTATION
#include "nuklear.h"

AppWindow::AppWindow(SDL_Rect frame)
{
    window = SDL_CreateWindow(nullptr, frame.x, frame.y, frame.w, frame.h, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    nk_context ctx;
    nk_init_fixed(&ctx, calloc(1, MAX_MEMORY), MAX_MEMORY, &font);
}

AppWindow::~AppWindow()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void AppWindow::SetProjectPath(std::wstring path)
{
}

void AppWindow::Render()
{
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}
