#pragma once
#include <Containers/String.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsBuffer.hpp>
#include <GraphicsFence.hpp>
#include <GraphicsSwapchain.hpp>
#include <GraphicsCommandlist.hpp>
#include <GraphicsBufferContainer.hpp>
#include <GraphicsPipeline.hpp>

namespace Faro
{
    struct GraphicsAdapterDesc
    {
        String name;
        String manufacturer;

        uint32 vram = 0;

        void* payload = nullptr;
    };

    class GraphicsAdapter
    {
    public:
        virtual void Init(GraphicsAdapterDesc&) = 0;
        virtual void Destroy() = 0;

        virtual GraphicsCommandList* CreateCommandList() = 0;

        virtual GraphicsBuffer* CreateBuffer(GraphicsBufferType type, GraphicsBufferDesc desc) = 0;

        template<class T>
        T* CreateBufferContainer(GraphicsBufferType type, GraphicsBufferDesc desc)
        {
            T* bufferContainer = MemoryManager::New<T>();
            bufferContainer->Init(this, type, desc);
            return bufferContainer;
        }

        GraphicsBufferContainer* CreateBufferContainer(GraphicsBufferType type, GraphicsBufferDesc desc)
        {
            return CreateBufferContainer<GraphicsBufferContainer>(type, desc);
        }

        virtual GraphicsFence* CreateFence() = 0;

        virtual GraphicsSwapchain* CreateSwapchain(Window* window) = 0;

        virtual GraphicsPipeline* CreatePipeline(GraphicsPipelineDesc desc) = 0;
    };
}
