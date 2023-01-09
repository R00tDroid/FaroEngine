#pragma once

namespace Faro
{
    #define RequireSubClass(BaseClass, SubClass) (std::is_base_of<BaseClass, SubClass>::value)

    //TODO Only compile in debug or development mode
    extern void _DebugCheck(bool expression, const char* expressionString, const char* message, const char* filename, int lineNumber, bool triggerAbort);

    #define Debug_BreakMessage(message) _DebugCheck(false, nullptr, message, __FILE__, __LINE__, true);
    #define Debug_Break() Debug_CheckMessage(false, nullptr);

    #define Debug_CheckMessage(expression, message) _DebugCheck(expression, #expression, message, __FILE__, __LINE__, false);
    #define Debug_Check(expression) Debug_CheckMessage(expression, nullptr);

    #define Debug_AssertMessage(expression, message) _DebugCheck(expression, #expression, message, __FILE__, __LINE__, true);
    #define Debug_Assert(expression) Debug_AssertMessage(expression, nullptr);

    #define Debug_BuildAssert(expression, message) static_assert(expression, message);
}
