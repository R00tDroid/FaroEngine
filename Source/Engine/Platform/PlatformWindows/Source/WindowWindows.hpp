#pragma once
#include <Window.hpp>

namespace Faro
{
    class WindowWindows : public Window
    {
    public:
        void Init() override;
        void Destroy() override;

        void SetTitle(String title) override;
        void SetSize(Int2D size) override;
        Int2D GetSize() override;
    };
}
