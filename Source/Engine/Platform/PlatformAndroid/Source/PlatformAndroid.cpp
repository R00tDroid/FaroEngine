#if FARO_OS_ANDROID
#include "PlatformAndroid.hpp"
#include <Log.hpp>
#include <WindowAndroid.hpp>
#include <Memory/MemoryManager.hpp>
#include <FileStreamAndroid.hpp>

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

            //TODO Log to console
        }
    };
    REGISTER_LOGSINK(LogCout)

    void PlatformAndroid::Init()
    {
    }

    void PlatformAndroid::Destroy()
    {
    }

    Window* PlatformAndroid::CreateWindow()
    {
        Window* window = MemoryManager::New<WindowAndroid>();
        window->Init();
        return window;
    }

    Array<Monitor> PlatformAndroid::GetMonitors()
    {
        return {};
    }

    FileStream* PlatformAndroid::OpenFile(Path file, EFileMode mode)
    {
        FileStreamAndroid* stream = MemoryManager::New<FileStreamAndroid>();
        stream->Init(file, mode);
        return stream;
    }

    Array<Path> PlatformAndroid::FindSubPaths(Path, bool, String)
    {
        return {};
    }
}
#endif
