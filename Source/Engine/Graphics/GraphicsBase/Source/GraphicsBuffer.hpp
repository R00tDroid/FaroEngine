#pragma once
#include <Primitives.hpp>
#include <Math/Vector2.hpp>
#include <GraphicsCommandList.hpp>

namespace Faro
{
    enum HeapType
    {
        HT_Upload,
        HT_Remote
    };

    enum HeapResourceType
    {
        HRT_Texture,
        HRT_ConstantBuffer,
        HRT_VertexBuffer,
        HRT_IndexBuffer
    };

    struct GraphicsHeapDesc 
    {
        HeapType type;
        HeapResourceType resourceType;
        uint32 dataSize = 0;
        uint32 stride = 0;

        struct TextureDesc
        {
            Int2D resolution = { 0, 0 };
            bool renderTarget = false;
            bool updateable = false;
            bool shaderResource = false;
        } texture;

        struct ConstantBufferDesc
        {
        } constantBuffer;

        struct GeometryBufferDesc
        {
        } geometryBuffer;
    };

    class GraphicsBuffer : public IGraphicsAdapterChild
    {
    public:
        const GraphicsHeapDesc& GetDesc();

        void Init(GraphicsAdapter* adapter, GraphicsCommandList* commandList, GraphicsHeapDesc desc);        

    private:
        GraphicsHeapDesc desc;
    };
}
