#pragma once
#include <GraphicsAdapter.hpp>

namespace Faro
{
    class GraphicsBufferVK : public GraphicsBuffer
    {
    public:
        void Destroy() override;

        void Upload(uint8* data) override;
    };
}
