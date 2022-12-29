#include "WindowsKitInfo.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

#ifdef WIN32
#include <Windows.h>
#include <comdef.h>
#endif

std::vector<WindowsKit> WindowsKits;
void FindWindowsKits()
{
    #ifdef WIN32
    if (WindowsKits.empty())
    {
        // Find root of windows kit installations
        HKEY Key;
        if (FAILED(RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows Kits\Installed Roots)", 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY, &Key)))
        {
            return;
        }

        // Enumerate installed versions
        char ValueData[MAX_PATH];
        DWORD ValueSize = MAX_PATH;
        if (SUCCEEDED(RegQueryValueExA(Key, "KitsRoot10", nullptr, nullptr, (LPBYTE)&ValueData, &ValueSize)))
        {
            std::string Kit10Root(ValueData, ValueSize - 1);

            int SubKeyIndex = 0;

            while (true)
            {
                char KeyName[MAX_PATH];
                DWORD KeyNameSize = MAX_PATH;
                if (RegEnumKeyExA(Key, SubKeyIndex, KeyName, &KeyNameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                SubKeyIndex++;

                std::string Version(KeyName, KeyNameSize);
                std::string BinPath = Kit10Root + "Bin\\" + Version;
                std::string LibPath = Kit10Root + "Lib\\" + Version;

                if (!std::filesystem::exists(BinPath) || !std::filesystem::exists(LibPath)) continue;

                WindowsKits.push_back({ Kit10Root, Version });
            }
        }

        RegCloseKey(Key);

        // Sort newest to oldest
        if (!WindowsKits.empty())
        {
            std::sort(WindowsKits.begin(), WindowsKits.end(), [](const WindowsKit& A, const WindowsKit& B)
                {
                    return B.Version < A.Version;
                });
        }
    }
    #endif
}

const std::vector<WindowsKit>& GetWindowsKits()
{
    FindWindowsKits();
    return WindowsKits;
}
