#include "WindowCreationModule.hpp"
#include <Log.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsInterface.hpp>
#include <Assert.hpp>

namespace Faro
{
    LOG_DECLARATION(LogWindowCreationTest, TestWindowcreation);

    void WindowCreationModule::Load()
    {
        LogWindowCreationTest.Log(LC_Info, "Creating window...");
        window = GPlatform->CreateWindow();
        Debug_Assert(window != nullptr);
        window->SetTitle("Test window");

        LogWindowCreationTest.Log(LC_Info, "Creating adapter...");
        Array<GraphicsAdapterDesc> adapters = GGraphics->GetAdapters();
        adapter = GGraphics->CreateAdapter(adapters[0]);
        Debug_Assert(adapter != nullptr);

        fence = adapter->CreateFence();
        Debug_Assert(fence != nullptr);

        swapchain = adapter->CreateSwapchain(window);
        Debug_Assert(swapchain != nullptr);

        GraphicsPipelineDesc pipelineDesc;
        pipeline = adapter->CreatePipeline(pipelineDesc);
        Debug_Assert(pipeline != nullptr);

        LogWindowCreationTest.Log(LC_Info, "Creating commandlist...");
        commandList = adapter->CreateCommandList();
        Debug_Assert(commandList != nullptr);
        commandList->Reset();

        LogWindowCreationTest.Log(LC_Info, "Creating buffers...");
        GraphicsBufferDesc bufferDesc = GraphicsBufferDesc::Texture2D({ 400, 300 }, true);
        uploadBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Upload, bufferDesc);
        remoteBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Remote, bufferDesc);
        Debug_Assert(uploadBuffer != nullptr);
        Debug_Assert(remoteBuffer != nullptr);

        commandList->SetResourceState(uploadBuffer, RS_CopySource);
        commandList->SetResourceState(remoteBuffer, RS_CopyDestination);

        commandList->CopyBuffer(uploadBuffer, remoteBuffer);

        commandList->SetResourceState(swapchain->backbuffer->Get(), RS_RenderTarget);
        commandList->ClearRenderTarget(swapchain->backbuffer->Get(), FloatColor::Green);
        commandList->SetResourceState(swapchain->backbuffer->Get(), RS_Present);

        LogWindowCreationTest.Log(LC_Info, "Execute commandlist");
        commandList->Execute();

        LogWindowCreationTest.Log(LC_Info, "Trigger fence");
        fence->Trigger();

        LogWindowCreationTest.Log(LC_Info, "Wait for fence...");
        fence->WaitForFinish();

        swapchain->Present();

        LogWindowCreationTest.Log(LC_Info, "Load finished");
    }

    void WindowCreationModule::Unload()
    {
        LogWindowCreationTest.Log(LC_Info, "Releasing graphics objects");
        MemoryManager::SafeDelete(pipeline);
        MemoryManager::SafeDelete(remoteBuffer);
        MemoryManager::SafeDelete(uploadBuffer);
        MemoryManager::SafeDelete(commandList);
        MemoryManager::SafeDelete(swapchain);
        MemoryManager::SafeDelete(fence);
        MemoryManager::SafeDelete(adapter);

        MemoryManager::SafeDelete(window);
    }

    String WindowCreationModule::GetName()
    {
        return "WindowCreation test";
    }

    Array<String> WindowCreationModule::GetRuntimeDependencies()
    {
        return { "Platform" };
    }
}
