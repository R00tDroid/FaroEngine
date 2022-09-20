#pragma once
#include <Memory/Object.hpp>

namespace Faro
{
    class GraphicsAdapter;

    class IGraphicsAdapterChild : public IObject
    {
    public:
        virtual void Init(GraphicsAdapter* adapter);

        GraphicsAdapter* GetAdapter();

        template<class T>
        T* GetTypedAdapter()
        {
            return (T*)GetAdapter();
        }

    protected:
        void Init() override;

    private:
        GraphicsAdapter* adapter = nullptr;
    };
}
