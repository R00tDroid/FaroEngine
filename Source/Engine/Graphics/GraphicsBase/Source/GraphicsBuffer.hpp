#pragma once
#include <Primitives.hpp>
#include <Math/Vector2.hpp>
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    /// @brief Describes the state of a graphics buffer
    enum GraphicsResourceState
    {
        RS_Unknown, ///< Unknown state. The buffer must be transitioned into another state before usage.
        RS_CopySource, ///< Source buffer for a copy operation.
        RS_CopyDestination, ///< Destination buffer for a copy operation.
        RS_RenderTarget, ///< Allows the buffer to be used as a rendertarget.
        RS_ShaderResource, ///< Allows the buffer to be used as a shader resource.
        RS_Present ///< Marks a rendertarget buffer read for presentation by its swapchain.
    };

    /// @brief Determines the type of a graphics buffer
    enum GraphicsBufferType
    {
        BT_Upload, ///< Uploads data from the CPU to the graphics adapter.
        BT_Remote ///< Stores memory on the graphics adapter.
    };

    /// @brief Describes the resource type of a graphics buffer.
    enum GraphicsResourceType
    {
        RT_Texture, //< The data should be treated as a texture.
        RT_ConstantBuffer, //< The data should be treated as a a constant buffer.
        RT_VertexBuffer, //< The data should be treated as mesh vertices.
        RT_IndexBuffer //< The data should be treated as mesh indices.
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
            bool updatable = false;
            bool shaderResource = false;
        } texture;

        struct ConstantBufferDesc
        {
        } constantBuffer;

        struct GeometryBufferDesc
        {
        } geometryBuffer;

        static GraphicsBufferDesc Texture2D(Int2D resolution, bool shaderResource = false, bool updatable = false, bool renderTarget = false)
        {
            GraphicsBufferDesc desc;
            desc.resourceType = RT_Texture;
            desc.dataSize = resolution.x * resolution.y * 4;
            desc.stride = resolution.x * 4;
            desc.texture.resolution = resolution;
            desc.texture.shaderResource = shaderResource;
            desc.texture.updatable = updatable;
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

        GraphicsResourceState GetResourceState();

    protected:
        void SetResourceState(GraphicsResourceState state);

    private:
        GraphicsBufferDesc desc;
        GraphicsResourceState state;
    };
}
