#include "MSVCInfo.hpp"

#include <string>
#include <vector>
#include <filesystem>
#include <glob/glob.hpp>

#ifdef WIN32
#include <Windows.h>
#include <comdef.h>
#include "Setup.Configuration.h"
#endif

#ifdef WIN32
_COM_SMARTPTR_TYPEDEF(ISetupConfiguration, __uuidof(ISetupConfiguration));
_COM_SMARTPTR_TYPEDEF(IEnumSetupInstances, __uuidof(IEnumSetupInstances));
_COM_SMARTPTR_TYPEDEF(ISetupInstance, __uuidof(ISetupInstance));
_COM_SMARTPTR_TYPEDEF(ISetupInstanceCatalog, __uuidof(ISetupInstanceCatalog));
_COM_SMARTPTR_TYPEDEF(ISetupPropertyStore, __uuidof(ISetupPropertyStore));
#endif
struct VSInstallation
{
    std::string Root;
    std::string Name;
};
std::vector<VSInstallation> VisualStudioInstallations;
void FindVisualStudioInstallations()
{
    #ifdef WIN32
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
    }
    #endif
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

            std::vector<std::filesystem::path> versions = glob::glob((std::filesystem::path(VSVersion.Root) / "VC" / "Redist" / "MSVC" / "v*").string());
            std::string msvcVersion = "";
            for (std::filesystem::path& version : versions)
            {
                msvcVersion = version.filename().string();
            }

            for (auto const& DirectoryEntry : std::filesystem::directory_iterator(MSVCDirectory))
            {
                if (DirectoryEntry.is_directory())
                {
                    std::filesystem::path Path = DirectoryEntry.path();
                    MSVCInstallations.push_back({ msvcVersion, Path.string() });
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
