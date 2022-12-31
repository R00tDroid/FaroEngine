#pragma once
#include <GraphicsAdapterChild.hpp>
#include <Primitives.hpp>
#include <Containers/Stream.hpp>
#include <GraphicsFormat.hpp>

namespace Faro
{
    /// @brief Describes a graphics pipeline.
    struct GraphicsPipelineDesc
    {
        /// @brief Pointer to a binary vertex shader.
        uint8* vsData = nullptr;
        /// @brief Size of the vertex shader data.
        uint16 vsSize = 0;

        /// @brief Pointer to a binary geometry shader.
        uint8* gsData = nullptr;   
        /// @brief Size of the geometry shader data.
        uint16 gsSize = 0;

        /// @brief Pointer to a binary pixel shader.
        uint8* psData = nullptr;
        /// @brief Size of the pixel shader data.
        uint16 psSize = 0;

        /// @brief Describes an input layout element.
        struct InputLayoutElement
        {
            /// @brief Element format.
            GraphicsFormat format;
            /// @brief Semantic name.
            String name;
            /// @brief Semantic index.
            uint8 semanticIndex = 0;
        };
        /// @brief List of input layout element.
        Array<InputLayoutElement> inputLayout;

        /// @brief List of rendertarget formats.
        Array<GraphicsFormat> renderTargets;
    };

    /// @brief Graphics pipeline state object
    class GraphicsPipeline : public IGraphicsAdapterChild
    {
    public:
        /**
         * @brief Load a prebuilt shader into the provided pipeline description.
         * @param stream Stream to read the shader from
         * @param[out] desc Pipeline description to place the shader in
         * @return true The shader was loaded successfully
         * @return false An error occurred while trying to load the shader
         */
        static bool LoadShader(DataStream* stream, GraphicsPipelineDesc& desc);

        using IGraphicsAdapterChild::Init;
        void Init(GraphicsAdapter* adapter, GraphicsPipelineDesc desc);

    protected:
        GraphicsPipelineDesc desc;
    };
}
