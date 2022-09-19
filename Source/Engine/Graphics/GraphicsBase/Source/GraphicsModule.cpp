#include "GraphicsModule.hpp"
#include <Log.hpp>
#include "GraphicsInterface.hpp"
#include "GraphicsLog.hpp"

namespace Faro
{
    void GraphicsModule::Load()
    {
        Log(GraphicsLog, LC_Debug, "Loading GGraphics");

        Array<GraphicsInterface*> graphicsInterfaces = GetGraphicsInterfaces();
        if (!graphicsInterfaces.Empty())
        {
            GGraphics = graphicsInterfaces[0];
            Log(GraphicsLog, LC_Info, "Platform: %s", GGraphics->GetName().Data());
            GGraphics->Init();
        }
        else
        {
            Log(GraphicsLog, LC_Error, "Failed to find a suitable graphics interface");
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
        return {};
    }
}
