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
    bool acceptingLogSinks = true;

    Array<LogSink> logSinks;
    void AddLogSink(LogSink logSink)
    {
        if (acceptingLogSinks) 
        {
            logSinks.Add(logSink);

            for (LogMessage& message : pendingLogMessages)
            {
                logSink(message.tag, message.category, message.message);
            }
        }
    }

    void LockLogSinks()
    {
        acceptingLogSinks = false;
        pendingLogMessages.Clear();
    }

    void Log(LogTag tag, ELogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        String message = FormatStringVA(format, args);
        va_end(args);

        if (acceptingLogSinks)
        {
            pendingLogMessages.Add({ tag, category, message });
        }

        for (LogSink& logSink : logSinks) 
        {
            logSink(tag, category, message);
        }
 
        if (category == LC_Fatal)
        {
            std::abort();
        }
    }
}
