#include "GraphicsInterface.hpp"

namespace Faro
{
    GraphicsInterface* GGraphics = nullptr;

    String GraphicsInterface::VendorCodeToString(uint32 vendorCode)
    {
        if (vendorCode == 0x1002)
        {
            return "AMD";
        }
        else if (vendorCode == 0x8086)
        {
            return "Intel";
        }
        else if (vendorCode == 0x10DE)
        {
            return "NVIDIA";
        }
        else if (vendorCode == 0x1414)
        {
            return "Microsoft";
        }
        return "Unknown";
    }

    Array<GraphicsInterface*> GetGraphicsInterfaces()
    {
        return {};// InstanceRegistry_GraphicsInterface::Get().GetValues();
    }
}
