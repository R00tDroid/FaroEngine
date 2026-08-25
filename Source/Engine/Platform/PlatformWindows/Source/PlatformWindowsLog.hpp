#pragma once
#include <Log.hpp>

namespace Faro
{
    LOG_DEFINITION(PlatformWindowsLog)

    Reflect(classFlag) class LogCout : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override;
    };

    Reflect(classFlag) class LogDebugOutput : public ILogSink
    {
    public:
        void Log(const LogMessage& message) override;
    };
}
