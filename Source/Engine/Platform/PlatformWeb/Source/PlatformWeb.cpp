#if FARO_OS_WEB
#include "PlatformWeb.hpp"
#include <iostream>
#include <Log.hpp>
#include <WindowWeb.hpp>
#include <Memory/MemoryManager.hpp>
#include <FileStreamWeb.hpp>

namespace Faro
{
    String FormatLogString(const LogTag& tag, const LogCategory& category, const String& message)
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

        return FormatString("[%-15s](%s) %s", tag.name.Data(), categoryLabel.Data(), message.Data());
    }

    class LogCout : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override
        {
            String string = FormatLogString(message.tag, message.category, message.message);

            if (message.category == LC_Error || message.category == LC_Fatal)
            {
                std::cerr << string.Data() << std::endl;
            }
            else 
            {
                std::cout << string.Data() << std::endl;
            }
        }
    };
    REGISTER_LOGSINK(LogCout)

    void PlatformWeb::Init()
    {
    }

    void PlatformWeb::Destroy()
    {
    }

    Window* PlatformWeb::CreateWindow()
    {
        Window* window = MemoryManager::New<WindowWeb>();
        window->Init();
        return window;
    }

    Array<Monitor> PlatformWeb::GetMonitors()
    {
        return {};
    }

    FileStream* PlatformWeb::OpenFile(Path file, EFileMode mode)
    {
        FileStreamWeb* stream = MemoryManager::New<FileStreamWeb>();
        stream->Init(file, mode);
        return stream;
    }

    Array<Path> PlatformWeb::FindSubPaths(Path, bool, String)
    {
        return {};
    }
}
#endif
