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

        static GraphicsBufferDesc Texture2D(Int2D resolution, bool shaderResource = false, bool updateable = false, bool renderTarget = false)
        {
            GraphicsBufferDesc desc;
            desc.resourceType = RT_Texture;
            desc.dataSize = resolution.x * resolution.y * 4;
            desc.stride = resolution.x * 4;
            desc.texture.resolution = resolution;
            desc.texture.shaderResource = shaderResource;
            desc.texture.updateable = updateable;
            desc.texture.renderTarget = shaderResource;
            return desc;
        }
    };

    class GraphicsBuffer : public IGraphicsAdapterChild
    {
    public:
        const GraphicsBufferDesc& GetDesc();

        void Init(GraphicsAdapter* adapter, GraphicsBufferDesc desc);     
        
        virtual void Upload(uint8* data) = 0;

    private:
        GraphicsBufferDesc desc;
    };
}
