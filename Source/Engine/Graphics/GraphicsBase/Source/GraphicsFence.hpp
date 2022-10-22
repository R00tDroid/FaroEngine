#pragma once
#include <GraphicsAdapterChild.hpp>

namespace Faro
{
    /// @brief Marker to be placed on an adapter command queue. Allows for synchronization between CPU and graphics adapter.
    class GraphicsFence : public IGraphicsAdapterChild
    {
    public:
        /// @brief Place a synchronization marker on the command queue of the related GraphicsAdapter.
        virtual void Trigger() = 0;

        /**
         * @brief Check wether or not the previously marker (Triggered by GraphicsFence::Trigger) has been hit yet.
         * @return true The marker has been hit
         * @return false The marker has not been hit yet
         */
        virtual bool HasFinished() = 0;

        /// @brief Wait for the previously marker (Triggered by GraphicsFence::Trigger) to be hit.
        virtual void WaitForFinish() = 0;
    };
}
