#pragma once
#include <Memory/Object.hpp>

namespace Faro
{
    class GraphicsAdapter;

    class IGraphicsAdapterChild : public IObject
    {
    public:
        /**
         * @brief Initialize this as a child object for an adapter. Also invokes IObject::Init, derived classes should use that for initialization.
         * @param adapter Graphics adapter to initialize this child on.
         */
        void Init(GraphicsAdapter* adapter);

        GraphicsAdapter* GetAdapter();

        template<class T>
        T* GetTypedAdapter()
        {
            return (T*)GetAdapter();
        }

        GraphicsAdapter* adapter = nullptr;

    protected:
        void Init() override;
    };
}
