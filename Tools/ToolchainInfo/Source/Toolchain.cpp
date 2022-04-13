#include "Toolchain.h"

#include <algorithm>
#include <string>
#include <vector>
#include <Windows.h>

struct WindowsKit
{
    std::string Root;
    std::string Version;
};
std::vector<WindowsKit> WindowsKits;
void FindWindowsKits()
{
    if (WindowsKits.empty())
    {
        // Find root of windows kit installations
        HKEY Key;
        if (FAILED(RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows Kits\Installed Roots)", 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY, &Key)))
        {
            return;
        }

        // Enumerate installed vesions
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

                // Validate if installed version is usable
                std::string Version(KeyName, KeyNameSize);
                std::string BinPath = Kit10Root + "Bin\\" + Version;
                std::string LibPath = Kit10Root + "Lib\\" + Version;

                WindowsKits.push_back({ Kit10Root, Version });

                SubKeyIndex++;
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
}

int CountAvailableWindowsKits()
{
    FindWindowsKits();

    return WindowsKits.size();
}

void GetAvailableWindowsKit(int Index, char** Root, char** Version)
{
    FindWindowsKits();

    if (Index >= 0 && Index < WindowsKits.size())
    {
        *Root = const_cast<char*>(WindowsKits[Index].Root.c_str());
        *Version = const_cast<char*>(WindowsKits[Index].Version.c_str());
    }
}
