#pragma once
#include <Memory/Object.hpp>

namespace Faro
{
    enum HeapType
    {
        HT_Upload,
        HT_Remote,
        HT_UploadToRemote
    };

    class IGraphicsHeap : public IObject
    {
    };

    class GraphicsUploadHeap : public IGraphicsHeap
    {
    };

    class GraphicsRemoteHeap : public IGraphicsHeap
    {
    };

    class GraphicsUploadToRemoteHeap : public IGraphicsHeap
    {
    };
}
