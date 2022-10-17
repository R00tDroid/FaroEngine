#pragma once
#include <Containers/String.hpp>
#include <Containers/Array.hpp>

namespace Faro
{
    enum ELogCategory
    {
        LC_Trace,
        LC_Debug,
        LC_Info,
        LC_Warning,
        LC_Error,
        LC_Fatal
    };

    struct LogTag
    {
        LogTag(String name);
        void Log(ELogCategory category, String format, ...);

        String name;
    };

#define LOG_DEFINITION(Tag) extern LogTag Tag;
#define LOG_DECLARATION(Tag, Header) LogTag Tag(#Header);

    typedef Function<void(const LogTag&, ELogCategory, const String&)> LogSink;

    class Logger
    {
    public:
        static void LogVA(const LogTag& tag, ELogCategory category, String format, va_list arguments);

        static void Log(const LogTag& tag, ELogCategory category, String format, ...);

        static void AddSink(LogSink logSink);
        static void LockSinks();

    private:
        static Array<LogSink> logSinks;
    };
}
