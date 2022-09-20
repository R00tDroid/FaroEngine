#pragma once

namespace Faro
{
    class GraphicsCommandList 
    {
    public:
        virtual void Reset() = 0;
        virtual void Execute() = 0;
    };
}
