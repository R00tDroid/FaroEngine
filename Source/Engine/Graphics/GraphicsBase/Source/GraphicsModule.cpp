#include "GraphicsModule.hpp"
#include <Log.hpp>
#include "GraphicsInterface.hpp"
#include "GraphicsLog.hpp"

namespace Faro
{
    void GraphicsModule::Load()
    {
        Logger::Log(GraphicsLog, LC_Debug, "Loading GGraphics");

        Array<GraphicsInterface*> graphicsInterfaces = GetGraphicsInterfaces();
        if (!graphicsInterfaces.Empty())
        {
            GGraphics = graphicsInterfaces[0];
            Logger::Log(GraphicsLog, LC_Info, "Graphics interface: %s", GGraphics->GetName().Data());
            GGraphics->Init();
        }
        else
        {
            Logger::Log(GraphicsLog, LC_Error, "Failed to find a suitable graphics interface");
        }
    }

    void GraphicsModule::Unload()
    {
        if (GGraphics != nullptr)
        {
            GGraphics->Destroy();
        }
    }

    String GraphicsModule::GetName()
    {
        return "Graphics";
    }

    Array<String> GraphicsModule::GetRuntimeDependencies()
    {
        return { "Platform" };
    }
}
