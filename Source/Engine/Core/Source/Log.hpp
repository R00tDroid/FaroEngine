#pragma once
#include <Containers/String.hpp>
#include <Containers/Array.hpp>

namespace Faro
{
    /// @brief Logging category
    enum ELogCategory
    {
        LC_Trace, ///< Tracing category. Provides in-depth insight of a sequence of events. Not visible to the end user.
        LC_Debug, ///< Debugging category. Shows debug info. Not visible to the end user.
        LC_Info, ///< Info category. Communicates generic informative messages.
        LC_Warning, ///< Warning category. Reports potential issues.
        LC_Error,  ///< Error category. Reports issues.
        LC_Fatal ///< Fatal category. Reports critical issues. This will also stop execution of the application.
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
