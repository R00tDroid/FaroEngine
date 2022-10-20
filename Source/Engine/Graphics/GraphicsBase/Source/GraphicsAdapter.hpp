#pragma once
#include <Containers/String.hpp>
#include <GraphicsBuffer.hpp>

namespace Faro
{
    struct GraphicsAdapterDesc
    {
        String name;
        String manufacturer;

        uint32 vram = 0;

        void* payload = nullptr;
    };

    class GraphicsCommandList;

    class GraphicsAdapter
    {
    public:
        virtual void Init(GraphicsAdapterDesc&) = 0;
        virtual void Destroy() = 0;

        virtual GraphicsCommandList* CreateCommandList() = 0;

        virtual GraphicsBuffer* CreateBuffer(GraphicsBufferDesc desc) = 0;
    };
}
