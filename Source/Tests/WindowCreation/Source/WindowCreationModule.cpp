#include "WindowCreationModule.hpp"
#include <Log.hpp>
#include <GraphicsInterface.hpp>
#include <Memory/MemoryManager.hpp>
#include <GraphicsCommandList.hpp>

namespace Faro
{
    LOG_DECLARATION(LogWindowCreationTest, TestWindowcreation);

    void WindowCreationModule::Load()
    {
        LogWindowCreationTest.Log(LC_Info, "Creating window...");
        window = GPlatform->CreateWindow();
        window->SetTitle("Test window");

        LogWindowCreationTest.Log(LC_Info, "Creating adapter...");
        Array<GraphicsAdapterDesc> adapters = GGraphics->GetAdapters();
        GraphicsAdapter* adapter = GGraphics->CreateAdapter(adapters[0]);

        GraphicsFence* fence = adapter->CreateFence();

        LogWindowCreationTest.Log(LC_Info, "Creating commandlist...");
        GraphicsCommandList* commandList = adapter->CreateCommandList();
        commandList->Reset();

        LogWindowCreationTest.Log(LC_Info, "Creating buffers...");
        GraphicsBufferDesc bufferDesc = GraphicsBufferDesc::Texture2D({ 400, 300 }, true, false, false);
        GraphicsBuffer* uploadBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Upload, bufferDesc);
        GraphicsBuffer* remoteBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Remote, bufferDesc);

        commandList->SetResourceState(uploadBuffer, RS_Read);
        commandList->SetResourceState(remoteBuffer, RS_CopyDestination);

        LogWindowCreationTest.Log(LC_Info, "Execute commandlsit");
        commandList->Execute();

        LogWindowCreationTest.Log(LC_Info, "Trigger fence");
        fence->Trigger();

        LogWindowCreationTest.Log(LC_Info, "Wait for fence...");
        fence->WaitForFinish();

        LogWindowCreationTest.Log(LC_Info, "Releasing graphics objects");
        MemoryManager::SafeDelete(remoteBuffer);
        MemoryManager::SafeDelete(uploadBuffer);
        MemoryManager::SafeDelete(commandList);
        MemoryManager::SafeDelete(fence);
        MemoryManager::SafeDelete(adapter);

        LogWindowCreationTest.Log(LC_Info, "Load finished");
    }

    void WindowCreationModule::Unload()
    {
        window->Destroy();
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
