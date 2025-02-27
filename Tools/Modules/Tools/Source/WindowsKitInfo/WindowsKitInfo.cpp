#include "WindowsKitInfo.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

#ifdef WIN32
#include <Windows.h>
#include <comdef.h>
#endif

std::vector<WindowsKit> foundWindowsKits;
void findWindowsKits()
{
    #ifdef WIN32
    if (foundWindowsKits.empty())
    {
        // Find root of windows kit installations
        HKEY Key;
        if (FAILED(RegOpenKeyExA(HKEY_LOCAL_MACHINE, R"(SOFTWARE\Microsoft\Windows Kits\Installed Roots)", 0, KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS | KEY_WOW64_64KEY, &Key)))
        {
            return;
        }

        // Enumerate installed versions
        char valueData[MAX_PATH];
        DWORD valueSize = MAX_PATH;
        if (SUCCEEDED(RegQueryValueExA(Key, "KitsRoot10", nullptr, nullptr, (LPBYTE)&valueData, &valueSize)))
        {
            std::string kit10Root(valueData, valueSize - 1);

            int subKeyIndex = 0;

            while (true)
            {
                char keyName[MAX_PATH];
                DWORD keyNameSize = MAX_PATH;
                if (RegEnumKeyExA(Key, subKeyIndex, keyName, &keyNameSize, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
                subKeyIndex++;

                std::string version(keyName, keyNameSize);
                std::string binPath = kit10Root + "Bin\\" + version;
                std::string libPath = kit10Root + "Lib\\" + version;

                if (!std::filesystem::exists(binPath) || !std::filesystem::exists(libPath)) continue;

                foundWindowsKits.push_back({ kit10Root, version });
            }
        }

        RegCloseKey(Key);

        // Sort newest to oldest
        if (!foundWindowsKits.empty())
        {
            std::sort(foundWindowsKits.begin(), foundWindowsKits.end(), [](const WindowsKit& A, const WindowsKit& B)
                {
                    return B.version < A.version;
                });
        }
    }
    #endif
}

unsigned int windowsKits()
{
    findWindowsKits();
    return static_cast<unsigned int>(foundWindowsKits.size());
}

WindowsKit windowsKit(unsigned int index)
{
    findWindowsKits();
    return foundWindowsKits[index];
}
