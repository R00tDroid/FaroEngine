#pragma once
#if FARO_OS_WINDOWS
#include <dxgi.h>
#include <GraphicsFormat.hpp>

namespace Faro
{
    extern DXGI_FORMAT Convert(GraphicsFormat format);
    extern GraphicsFormat Convert(DXGI_FORMAT format);
}
#endif
