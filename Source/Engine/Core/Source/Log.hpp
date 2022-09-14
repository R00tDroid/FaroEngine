#pragma once
#include <functional>
#include <Containers/String.h>

namespace Faro
{
    typedef String LogTag;
#define LOG_DEFINITION(Tag) extern LogTag Tag;
#define LOG_DECLARATION(Tag) LogTag Tag;

    enum ELogCategory
    {
        LC_Trace,
        LC_Debug,
        LC_Info,
        LC_Warning,
        LC_Error,
        LC_Fatal
    };

    extern void Log(LogTag tag, ELogCategory category, String format, ...);

    extern void SetLogSink(std::function<void(LogTag, ELogCategory, String)> logSink);
}