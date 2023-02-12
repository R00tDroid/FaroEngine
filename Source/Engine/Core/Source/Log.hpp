#pragma once
#include <Containers/String.hpp>
#include <Containers/Array.hpp>
#include <Util/ClassRegistry.hpp>

namespace Faro
{
    /// @brief Logging category
    enum LogCategory
    {
        LC_Trace, ///< Tracing category. Provides in-depth insight of a sequence of events. Not visible to the end user.
        LC_Debug, ///< Debugging category. Shows debug info. Not visible to the end user.
        LC_Info, ///< Info category. Communicates generic informative messages.
        LC_Warning, ///< Warning category. Reports potential issues.
        LC_Error,  ///< Error category. Reports issues.
        LC_Fatal ///< Fatal category. Reports critical issues. This will also stop execution of the application.
    };

    /// @brief Defines a logging tag. To be defined and declared with LOG_DEFINITION and LOG_DECLARATION.
    struct LogTag
    {
        LogTag(String name);

        /**
         * @brief Log a message with this tag
         * @param category Log category
         * @param format Format string
         * @param ... List of argument to be used for formatting
         */
        void Log(LogCategory category, String format, ...);

        /// @brief Name of this log tag
        String name;
    };

#define LOG_DEFINITION(Tag) extern LogTag Tag;
#define LOG_DECLARATION(Tag, Header) LogTag Tag(#Header);

    /// @internal
    struct LogMessage
    {
        const LogTag& tag;
        LogCategory category;
        String message;
    };

    class ILogSink
    {
    public:
        /// @brief Logging callback that is executed by Logger.
        virtual void Log(const LogMessage& message) = 0;
    };

    /// @brief Class responsible for all logging related functionality.
    class Logger
    {
    public:
        static void Init();

        static void Destroy();

        /**
         * @brief Log a messages with the provided format string and variadic argument list.
         * @param tag Log tag
         * @param category Log category
         * @param format Format string
         * @param arguments Variadic argument list to be used for formatting
         */
        static void LogVA(const LogTag& tag, LogCategory category, String format, va_list arguments);

        /**
         * @brief Log a messages with the provided format string and arguments.
         * @param tag Log tag
         * @param category Log category
         * @param format Format string
         * @param ... List of argument to be used for formatting
         */
        static void Log(const LogTag& tag, LogCategory category, String format, ...);

    private:
        static Array<ILogSink*> logSinks;
    };

    DEFINE_INSTANCE_REGISTRY(LogSinks, Faro::ILogSink)
    #define REGISTER_LOGSINK(SINK) REGISTER_INSTANCE(LogSinks, SINK)
}
