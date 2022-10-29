#pragma once
#include <string>
#include <vector>

struct WindowsKit
{
    std::string Root;
    std::string Version;
};

extern const std::vector<WindowsKit>& GetWindowsKits();

int CountWindowsKits();
char* GetWindowsKitRoot(int Index);
char* GetWindowsKitVersion(int Index);


struct MSVCVersion
{
    std::string RedistVersion;
    std::string Root;
};

extern const std::vector<MSVCVersion>& GetMSVCInstallations();

int CountMSVC();
char* GetMSVCRoot(int Index);
