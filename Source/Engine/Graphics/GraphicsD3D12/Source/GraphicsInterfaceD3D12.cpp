#include "GraphicsInterfaceD3D12.hpp"
#include <Windows.h>
#include <d3d12.h>
#include <comdef.h>
#include "GraphicsAdapterD3D12.hpp"
#include "GraphicsLogD3D12.hpp"
#include "Memory/MemoryManager.hpp"

namespace Faro
{
    bool GraphicsInterfaceD3D12::Init()
    {
#ifndef NDEBUG
        ID3D12Debug* DebugInterface = nullptr;
        if (D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface)) == S_OK)
        {
            Logger::Log(GraphicsLogD3D12, LC_Warning, "Enabling debug layer");
            DebugInterface->EnableDebugLayer();
            DebugInterface->Release();
        }
#endif

        DWORD DXGIFactoryFlags = 0;
#ifndef NDEBUG
        DXGIFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
        CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

        DetectAdapters();

        return true;
    }

    void GraphicsInterfaceD3D12::Destroy()
    {
        for (GraphicsAdapterDesc& desc : adapterDescs)
        {
            IDXGIAdapter1* adapter = static_cast<IDXGIAdapter1*>(desc.payload);
            adapter->Release();
        }
        adapterDescs.Clear();
    }

    Array<GraphicsAdapterDesc> GraphicsInterfaceD3D12::GetAdapters()
    {
        return adapterDescs;
    }

    GraphicsAdapter* GraphicsInterfaceD3D12::CreateAdapter(GraphicsAdapterDesc description)
    {
        GraphicsAdapterD3D12* adapter = MemoryManager::New<GraphicsAdapterD3D12>();
        adapter->Init(description);
        return adapter;
    }

    IDXGIFactory4* GraphicsInterfaceD3D12::GetFactory()
    {
        return dxgiFactory;
    }

    void GraphicsInterfaceD3D12::DetectAdapters()
    {
        Logger::Log(GraphicsLogD3D12, LC_Debug, "Looking for graphics adapter...");
        adapterDescs = {};

        IDXGIAdapter* adapter = nullptr;

        for (UINT i = 0; dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
        {
            DXGI_ADAPTER_DESC adapterDesc;
            if (FAILED(adapter->GetDesc(&adapterDesc)))
            {
                break;
            }

            GraphicsAdapterDesc desc;
            desc.name = static_cast<char*>(_bstr_t(adapterDesc.Description));
            desc.vram = (uint32)adapterDesc.DedicatedVideoMemory;
            desc.payload = adapter;

            if (adapterDesc.VendorId == 0x1002)
            {
                desc.manufacturer = "AMD";
            }
            else if (adapterDesc.VendorId == 0x8086)
            {
                desc.manufacturer = "Intel";
            }
            else if (adapterDesc.VendorId == 0x10DE)
            {
                desc.manufacturer = "NVidia";
            }
            else if (adapterDesc.VendorId == 0x1414)
            {
                desc.manufacturer = "Microsoft";
            }

            if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }

            adapterDescs.Add(desc);

            Logger::Log(GraphicsLogD3D12, LC_Debug, "Detect graphics adapter: %s (%umb)", desc.name.Data(), desc.vram / 1024 / 1024);
        }
    }
}
