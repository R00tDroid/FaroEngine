#pragma once
#include <Primitives.hpp>
#include <Math/Vector2.hpp>
#include <GraphicsCommandList.hpp>

namespace Faro
{
    enum GraphicsBufferType
    {
        BT_Upload,
        BT_Remote
    };

    enum GraphicsResourceType
    {
        RT_Texture,
        RT_ConstantBuffer,
        RT_VertexBuffer,
        RT_IndexBuffer
    };

    struct GraphicsBufferDesc 
    {
        GraphicsResourceType resourceType;
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
        const GraphicsBufferDesc& GetDesc();

        void Init(GraphicsAdapter* adapter, GraphicsBufferDesc desc);        

    private:
        GraphicsBufferDesc desc;
    };
}
