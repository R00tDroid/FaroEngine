#include "ToolchainInfo.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <Windows.h>
#include <comdef.h>
#include <filesystem>

#include "Setup.Configuration.h"


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
}

const std::vector<WindowsKit>& GetWindowsKits()
{
    FindWindowsKits();
    return WindowsKits;
}

int CountWindowsKits()
{
    FindWindowsKits();

    return (int)WindowsKits.size();
}

char* GetWindowsKitRoot(int Index)
{
    FindWindowsKits();

    if (Index >= 0 && Index < WindowsKits.size())
    {
        return const_cast<char*>(WindowsKits[Index].Root.c_str());
    }

    return nullptr;
}

char* GetWindowsKitVersion(int Index)
{
    FindWindowsKits();

    if (Index >= 0 && Index < WindowsKits.size())
    {
        return const_cast<char*>(WindowsKits[Index].Version.c_str());
    }

    return nullptr;
}



_COM_SMARTPTR_TYPEDEF(ISetupConfiguration, __uuidof(ISetupConfiguration));
_COM_SMARTPTR_TYPEDEF(IEnumSetupInstances, __uuidof(IEnumSetupInstances));
_COM_SMARTPTR_TYPEDEF(ISetupInstance, __uuidof(ISetupInstance));
_COM_SMARTPTR_TYPEDEF(ISetupInstanceCatalog, __uuidof(ISetupInstanceCatalog));
_COM_SMARTPTR_TYPEDEF(ISetupPropertyStore, __uuidof(ISetupPropertyStore));
struct VSInstallation
{
    std::string Root;
    std::string Name;
};
std::vector<VSInstallation> VisualStudioInstallations;
void FindVisualStudioInstallations()
{
    if (VisualStudioInstallations.empty())
    {
        ISetupConfigurationPtr SetupConfig;
        IEnumSetupInstancesPtr InstanceEnumerator;

        auto UserLCID = GetUserDefaultLCID();

        CoInitialize(nullptr);
        SetupConfig.CreateInstance(__uuidof(SetupConfiguration));
        SetupConfig->EnumInstances(&InstanceEnumerator);

        while (true)
        {
            ISetupInstance* InstancePtr = nullptr;
            unsigned long DummyValue = 0;
            HRESULT hr = InstanceEnumerator->Next(1, &InstancePtr, &DummyValue);
            if (hr != S_OK)
            {
                break;
            }

            ISetupInstancePtr SetupInstance(InstancePtr, false);
            ISetupInstanceCatalogPtr InstanceCatalog;
            ISetupPropertyStorePtr InfoStorage;
            SetupInstance->QueryInterface(&InstanceCatalog);
            InstanceCatalog->GetCatalogInfo(&InfoStorage);

            VSInstallation VSInfo;

            BSTR InstallPath;
            SetupInstance->GetInstallationPath(&InstallPath);
            VSInfo.Root = _bstr_t(InstallPath);

            BSTR DisplayName;
            SetupInstance->GetDisplayName(UserLCID, &DisplayName);
            VSInfo.Name = _bstr_t(DisplayName);

            if (!std::filesystem::exists(VSInfo.Root)) continue;

            VisualStudioInstallations.push_back(VSInfo);
        }
        CoUninitialize();
    }
}

std::vector<MSVCVersion> MSVCInstallations;
void FindMSVCInstallations()
{
    if (MSVCInstallations.empty()) 
    {
        FindVisualStudioInstallations();
        for (const VSInstallation& VSVersion : VisualStudioInstallations)
        {
            std::string MSVCDirectory = VSVersion.Root + "\\VC\\Tools\\MSVC";

            for (auto const& DirectoryEntry : std::filesystem::directory_iterator(MSVCDirectory))
            {
                if (DirectoryEntry.is_directory())
                {
                    std::filesystem::path Path = DirectoryEntry.path();
                    MSVCInstallations.push_back({ Path.string() });
                }
            }
        }
    }
}

const std::vector<MSVCVersion>& GetMSVCInstallations()
{
    FindMSVCInstallations();
    return MSVCInstallations;
}

int CountMSVC()
{
    FindMSVCInstallations();
    return (int)MSVCInstallations.size();
}

char* GetMSVCRoot(int Index)
{
    FindMSVCInstallations();

    if (Index >= 0 && Index < MSVCInstallations.size())
    {
        return const_cast<char*>(MSVCInstallations[Index].Root.c_str());
    }

    return nullptr;
}
