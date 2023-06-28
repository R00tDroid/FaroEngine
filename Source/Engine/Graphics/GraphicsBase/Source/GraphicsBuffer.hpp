#pragma once
#include <GraphicsAdapterChild.hpp>
#include <GraphicsBufferDesc.hpp>

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

    /// @brief A buffer storing graphics related data. Created by GraphicsAdapter::CreateBuffer.
    class GraphicsBuffer : public IGraphicsAdapterChild
    {
    public:
        /**
         * @brief Get the description of this buffer.
         * @return const GraphicsBufferDesc& Buffer description
         */
        const GraphicsBufferDesc& GetDesc();

        void Init(GraphicsAdapter* adapter, GraphicsBufferDesc desc);

        /**
         * @brief Prepare the given data so it can be uploaded to the graphics adapter. This buffer **must** be created as BT_Upload!
         * @param data Pointer to the data to upload
         */
        virtual void Upload(uint8* data) = 0;

        /**
         * @brief Get the current resource state of this buffer.
         * @return GraphicsResourceState Current resource state
         */
        GraphicsResourceState GetResourceState();

    protected:
        /**
         * @brief Set the resource state flag for internal referencing.
         * @param state New resource state
         */
        void SetResourceState(GraphicsResourceState state);

    private:
        GraphicsBufferDesc desc;
        GraphicsResourceState state;
    };
}
