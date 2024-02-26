#pragma once
#include <Primitives.hpp>
#include <Math/Vector2.hpp>

namespace Faro
{
    class GraphicsSwapchain;

    /// @brief Describes the resource type of a graphics buffer.
    enum GraphicsResourceType
    {
        RT_Texture, //< The data should be treated as a texture.
        RT_ConstantBuffer, //< The data should be treated as a a constant buffer.
        RT_VertexBuffer, //< The data should be treated as mesh vertices.
        RT_IndexBuffer //< The data should be treated as mesh indices.
    };

    /// @brief Description of a graphics buffer
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

        struct RenderTargetDesc
        {
            GraphicsSwapchain* swapchain = nullptr;
            uint8 swapchainImageIndex = 0;
        } renderTarget;

        struct ConstantBufferDesc
        {
        } constantBuffer;

        struct GeometryBufferDesc
        {
        } geometryBuffer;

        static GraphicsBufferDesc Texture2D(Int2D resolution, bool shaderResource = false, bool updatable = false)
        {
            GraphicsBufferDesc desc;
            desc.resourceType = RT_Texture;
            desc.dataSize = resolution.x * resolution.y * 4;
            desc.stride = resolution.x * 4;
            desc.texture.resolution = resolution;
            desc.texture.shaderResource = shaderResource;
            desc.texture.updatable = updatable;
            return desc;
        }

        static GraphicsBufferDesc RenderTarget(Int2D resolution, bool shaderResource = false)
        {
            GraphicsBufferDesc desc;
            desc.resourceType = RT_Texture;
            desc.dataSize = resolution.x * resolution.y * 4;
            desc.stride = resolution.x * 4;
            desc.texture.resolution = resolution;
            desc.texture.shaderResource = shaderResource;
            desc.texture.renderTarget = true;
            return desc;
        }

        static GraphicsBufferDesc SwapchainImage(GraphicsSwapchain* swapchain, uint8 imageIndex)
        {
            GraphicsBufferDesc desc;
            desc.resourceType = RT_Texture;
            desc.texture.renderTarget = true;
            desc.renderTarget.swapchain = swapchain;
            desc.renderTarget.swapchainImageIndex = imageIndex;
            return desc;
        }
    };

    /// @brief Determines the type of a graphics buffer
    enum GraphicsBufferType
    {
        BT_Upload, ///< Uploads data from the CPU to the graphics adapter.
        BT_Remote ///< Stores memory on the graphics adapter.
    };

    struct GraphicsBufferCreateDesc
    {
        GraphicsBufferType bufferType;
        GraphicsBufferDesc bufferDesc;
    };
}
