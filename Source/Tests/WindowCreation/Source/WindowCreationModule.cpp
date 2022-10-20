#include "WindowCreationModule.hpp"
#include <Log.hpp>
#include <GraphicsInterface.hpp>

#include "Memory/MemoryManager.hpp"

namespace Faro
{
    void WindowCreationModule::Load()
    {
        window = GPlatform->CreateWindow();
        window->SetTitle("Test window");

        Array<GraphicsAdapterDesc> adapters = GGraphics->GetAdapters();
        GraphicsAdapter* adapter = GGraphics->CreateAdapter(adapters[0]);

        GraphicsCommandList* commandList = adapter->CreateCommandList();

        GraphicsBufferDesc bufferDesc = GraphicsBufferDesc::Texture2D({ 400, 300 }, true, false, false);
        GraphicsBuffer* uploadBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Upload, bufferDesc);
        GraphicsBuffer* remoteBuffer = adapter->CreateBuffer(GraphicsBufferType::BT_Remote, bufferDesc);

        MemoryManager::SafeDelete(remoteBuffer);
        MemoryManager::SafeDelete(uploadBuffer);
        MemoryManager::SafeDelete(commandList);
        MemoryManager::SafeDelete(adapter);
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
