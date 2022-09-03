#pragma once
#include "../Containers/String.h"
#include "../Containers/Array.h"
#include <stdarg.h>
#include "../Memory/MemoryManager.h"

namespace Faro
{
    extern bool MatchPattern(String pattern, String source);
    extern Array<String> ParsePattern(String pattern, String source);

    extern Array<String> SplitString(String source, char delimiter);
    extern Array<String> SplitString(String source, String delimiter);

    extern String ToLowerCase(String);
    extern String ToUpperCase(String);

    inline String FormatString(String format, ...)
    {
        int size = 512;
        char* buffer = 0;
        buffer = (char*)MemoryManager::Alloc(size);
        va_list vl;
        va_start(vl, format);
        int nsize = vsnprintf(buffer, size, *format, vl);
        if (size <= nsize)
        {
            MemoryManager::Free(buffer);
            buffer = nullptr;
            buffer = (char*)MemoryManager::Alloc(nsize + 1);
            nsize = vsnprintf(buffer, size, *format, vl);
        }
        std::string ret(buffer);
        va_end(vl);
        MemoryManager::Free(buffer);
        return ret;
    }
}
