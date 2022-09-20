#pragma once

namespace Faro
{
    class IObject
    {
    public:
        virtual void Init();

        // @warning Must be called last
        virtual void Destroy();
    };
}
