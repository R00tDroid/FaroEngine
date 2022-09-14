#include "Log.hpp"

namespace Faro
{
    void Log(LogTag tag, ELogCategory category, String format, ...)
    {
        va_list args;
        va_start(args, format);
        String message = FormatStringVA(format, args);
        va_end(args);
    }
}
