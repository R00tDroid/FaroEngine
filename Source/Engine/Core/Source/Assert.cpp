#pragma once
#include <Assert.hpp>
#include <cstdlib>

#include <iostream>
#if 1==1 //TODO Properly check platform
#include <sstream>
#include <windows.h>
#define DebugOutputMessage(message)           \
{                                             \
    std::cerr << message;                     \
    std::ostringstream stream;                \
    stream << message;                        \
    OutputDebugStringA(stream.str().c_str()); \
}
#else
#define DebugOutputMessage(message)  \
{                                    \
    std::cerr << message;            \
}
#endif

namespace Faro
{
    void _DebugCheck(bool expression, const char* expressionString, const char* message, const char* filename, int lineNumber, bool triggerAbort)
    {
        if (!expression)
        {
            if (message == nullptr)
            {
                DebugOutputMessage("\n--Debug check triggered--\n" << filename << "(" << lineNumber << ")\n");
                if (expressionString != nullptr) DebugOutputMessage("Expression: " << expressionString << "\n");
                if (message != nullptr) DebugOutputMessage("Message: " << expressionString << "\n\n");
            }

            if (triggerAbort)
            {
                DebugOutputMessage("--Aborting--\n\n");
                abort();
            }
        }
    }
}
