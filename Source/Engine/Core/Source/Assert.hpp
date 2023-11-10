#pragma once

#include <type_traits>
#include <iostream>

#ifdef FARO_DEBUG // Define internal debugging macros

    #ifdef FARO_OS_WINDOWS
    #include <sstream>
    #include <MinWindows.hpp>
    #define DebugOutputMessage(message)           \
    {                                             \
        std::cerr << message;                     \
        std::ostringstream stream;                \
        stream << message;                        \
        OutputDebugStringA(stream.str().c_str()); \
    }
    #define Debug_Break() __debugbreak()
    #else
    #define DebugOutputMessage(message)  \
    {                                    \
        std::cerr << message;            \
    }

    #define Debug_Break() raise(SIGTRAP)
    #endif

#define _DebugCheck(expression, expressionString, message, filename, lineNumber, triggerAbort)\
    if (!(expression)){\
        DebugOutputMessage("\n--Debug check triggered--\n");\
        DebugOutputMessage((filename) << ":" << (lineNumber) << "\n");\
        if ((expressionString) != nullptr) DebugOutputMessage("Expression: " << (expressionString) << "\n");\
        if ((message) != nullptr) DebugOutputMessage("Message: " << (expressionString) << "\n\n");\
        if ((triggerAbort)) Debug_Break();\
    }

#else // Define placeholder for non-debug mode
#define _DebugCheck(expression, expressionString, message, filename, lineNumber, triggerAbort)
#define Debug_Break()
#endif

// Define usable macros
#define RequireSubClass(BaseClass, SubClass) (std::is_base_of<BaseClass, SubClass>::value)

#define Debug_BreakMessage(message) _DebugCheck(false, nullptr, message, __FILE__, __LINE__, true);

#define Debug_CheckMessage(expression, message) _DebugCheck(expression, #expression, message, __FILE__, __LINE__, false);
#define Debug_Check(expression) Debug_CheckMessage(expression, nullptr);

#define Debug_AssertMessage(expression, message) _DebugCheck(expression, #expression, message, __FILE__, __LINE__, true);
#define Debug_Assert(expression) Debug_AssertMessage(expression, nullptr);

#define Debug_BuildAssert(expression, message) static_assert(expression, message);
