#include "Log.hpp"

namespace Faro
{
    struct LogMessage
    {
        const LogTag& tag;
        ELogCategory category;
        String message;
    };
    Array<LogMessage> pendingLogMessages;
    bool acceptingLogSinks = true;

    Array<LogSink> Logger::logSinks;

    void Logger::AddSink(LogSink logSink)
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

    void Logger::LockSinks()
    {
        acceptingLogSinks = false;
        pendingLogMessages.Clear();
    }

    LogTag::LogTag(String inName) : name(inName) {}

    void LogTag::Log(ELogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        Logger::LogVA(*this, category, format, args);
        va_end(args);
    }

    void Logger::LogVA(const LogTag& tag, ELogCategory category, String format, va_list arguments)
    {
        String message = FormatStringVA(format, arguments);

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

    void Logger::Log(const LogTag& tag, ELogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        LogVA(tag, category, format, args);
        va_end(args);
    }
}
