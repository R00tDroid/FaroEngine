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
    std::string root;
    std::string name;
};
std::vector<VSInstallation> visualStudioInstallations;
void findVisualStudioInstallations()
{
    #ifdef WIN32
    if (visualStudioInstallations.empty())
    {
        ISetupConfigurationPtr setupConfig;
        IEnumSetupInstancesPtr instanceEnumerator;

        auto userLCID = GetUserDefaultLCID();

        HRESULT initResult = CoInitialize(nullptr);
        if (initResult != S_OK) return;

        setupConfig.CreateInstance(__uuidof(SetupConfiguration));
        setupConfig->EnumInstances(&instanceEnumerator);

        while (true)
        {
            ISetupInstance* instancePtr = nullptr;
            unsigned long dummyValue = 0;
            HRESULT hr = instanceEnumerator->Next(1, &instancePtr, &dummyValue);
            if (hr != S_OK)
            {
                break;
            }

            ISetupInstancePtr setupInstance(instancePtr, false);
            ISetupInstanceCatalogPtr instanceCatalog;
            ISetupPropertyStorePtr infoStorage;
            setupInstance->QueryInterface(&instanceCatalog);
            instanceCatalog->GetCatalogInfo(&infoStorage);

            VSInstallation vsInstall;

            BSTR InstallPath;
            setupInstance->GetInstallationPath(&InstallPath);
            vsInstall.root = _bstr_t(InstallPath);

            BSTR displayName;
            setupInstance->GetDisplayName(userLCID, &displayName);
            vsInstall.name = _bstr_t(displayName);

            if (!std::filesystem::exists(vsInstall.root)) continue;

            visualStudioInstallations.push_back(vsInstall);
        }
    }
    #endif
}

std::vector<MSVCVersion> msvcInstallations;
void findMSVCInstallations()
{
    if (msvcInstallations.empty())
    {
        findVisualStudioInstallations();
        for (const VSInstallation& vsInstall : visualStudioInstallations)
        {
            std::string msvcDirectory = vsInstall.root + "\\VC\\Tools\\MSVC";

            std::vector<std::filesystem::path> versions = glob::glob((std::filesystem::path(vsInstall.root) / "VC" / "Redist" / "MSVC" / "v*").string());
            std::string msvcVersion = "";
            for (std::filesystem::path& version : versions)
            {
                msvcVersion = version.filename().string();
            }

            for (auto const& directoryEntry : std::filesystem::directory_iterator(msvcDirectory))
            {
                if (directoryEntry.is_directory())
                {
                    std::filesystem::path path = directoryEntry.path();
                    msvcInstallations.push_back({ msvcVersion, path.string() });
                }
            }
        }
    }
}

const std::vector<MSVCVersion>& getMSVCInstallations()
{
    findMSVCInstallations();
    return msvcInstallations;
}
