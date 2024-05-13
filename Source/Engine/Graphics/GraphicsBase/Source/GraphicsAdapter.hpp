#pragma once
#include <Containers/String.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsBuffer.hpp>
#include <GraphicsFence.hpp>
#include <GraphicsSwapchain.hpp>
#include <GraphicsCommandList.hpp>
#include <GraphicsBufferContainer.hpp>
#include <GraphicsPipeline.hpp>
#include <GraphicsSemaphore.hpp>

namespace Faro
{
    struct GraphicsAdapterDesc
    {
        String name;
        String manufacturer;

        uint64 vramDedicated = 0;
        uint64 vramShared = 0;
        uint64 vramTotal = 0;

        void* payload = nullptr;
    };

    class GraphicsAdapter
    {
    public:
        virtual void Init(GraphicsAdapterDesc&) = 0;
        virtual void Destroy() = 0;

        virtual GraphicsCommandList* CreateCommandList() = 0;

        virtual GraphicsBuffer* CreateBuffer(GraphicsBufferCreateDesc createDesc) = 0;

        template<class T>
        T* CreateBufferContainer(GraphicsBufferCreateDesc createDesc)
        {
            T* bufferContainer = MemoryManager::New<T>();
            bufferContainer->Init(this, createDesc);
            return bufferContainer;
        }

        GraphicsBufferContainer* CreateBufferContainer(GraphicsBufferCreateDesc createDesc)
        {
            return CreateBufferContainer<GraphicsBufferContainer>(createDesc);
        }

        virtual GraphicsFence* CreateFence() = 0;

        virtual GraphicsSwapchain* CreateSwapchain(Window* window) = 0;

        virtual GraphicsPipeline* CreatePipeline(GraphicsPipelineDesc desc) = 0;

        virtual GraphicsSemaphore* CreateSyncPoint();
    };
}
