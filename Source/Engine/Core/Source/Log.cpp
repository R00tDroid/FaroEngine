#include "Log.hpp"

namespace Faro
{
    IMPLEMENT_INSTANCE_REGISTRY(LogSinks, Faro::ILogSink)

    Array<ILogSink*> Logger::logSinks;

    Array<ILogSink*> GetRegisteredLogSinks()
    {
        return InstanceRegistry_LogSinks::Get().GetValues();
    }

    LogTag::LogTag(String inName) : name(inName) {}

    void LogTag::Log(LogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        Logger::LogVA(*this, category, format, args);
        va_end(args);
    }

    void Logger::Init()
    {
        logSinks = GetRegisteredLogSinks();
    }

    void Logger::Destroy()
    {
        logSinks.Empty();
    }

    void Logger::LogVA(const LogTag& tag, LogCategory category, String format, va_list arguments)
    {
        LogMessage message { tag, category, FormatStringVA(format, arguments) };

        for (ILogSink* logSink : logSinks)
        {
            logSink->Log(message);
        }

        if (category == LC_Fatal)
        {
            std::abort();
        }
    }

    void Logger::Log(const LogTag& tag, LogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        LogVA(tag, category, format, args);
        va_end(args);
    }
}
