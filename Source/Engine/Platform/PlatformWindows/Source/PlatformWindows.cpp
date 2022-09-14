#include "PlatformWindows.hpp"
#include <iostream>
#include <Log.hpp>
#include <Windows.h>

namespace Faro
{
    void LogWindows(LogTag tag, ELogCategory category, String message)
    {
        String categoryLabel = "";

        switch (category)
        {
            case LC_Trace: { categoryLabel = "T"; break; }
            case LC_Debug: { categoryLabel = "D"; break; }
            case LC_Info: { categoryLabel = "I"; break; }
            case LC_Warning: { categoryLabel = "W"; break; }
            case LC_Error: { categoryLabel = "E"; break; }
            case LC_Fatal: { categoryLabel = "F"; break; }
        }

        String result = FormatString("[%-15s](%s) %s", tag.Data(), categoryLabel.Data(), message.Data());
        std::cout << result.Data() << std::endl;
        OutputDebugStringA((result + "\n").Data());
    }

    void PlatformWindows::Init()
    {
        SetLogSink(LogWindows);
    }

    void PlatformWindows::Destroy()
    {
    }
}
