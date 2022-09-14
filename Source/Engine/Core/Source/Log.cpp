#include "Log.hpp"
#include <Containers/Array.hpp>

namespace Faro
{
    struct LogMessage
    {
        LogTag tag;
        ELogCategory category;
        String message;
    };
    Array<LogMessage> pendingLogMessages;

    std::function<void(LogTag, ELogCategory, String)> logSink = nullptr;
    void SetLogSink(std::function<void(LogTag, ELogCategory, String)> inLogSink)
    {
        logSink = inLogSink;
        for (LogMessage& message : pendingLogMessages)
        {
            logSink(message.tag, message.category, message.message);
        }
        pendingLogMessages.Clear();
    }
    
    void Log(LogTag tag, ELogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        String message = FormatStringVA(format, args);
        va_end(args);

        if (logSink == nullptr)
        {
            pendingLogMessages.Add({ tag, category, message });
        }
        else
        {
            logSink(tag, category, message);
        }
        if (category == LC_Fatal)
        {
            std::abort();
        }
    }
}
