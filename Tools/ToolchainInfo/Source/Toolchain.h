#pragma once
#include "Dll.h"

DllExport int CountWindowsKits();
DllExport char* GetWindowsKitRoot(int Index);
DllExport char* GetWindowsKitVersion(int Index);

DllExport int CountMSVC();
DllExport char* GetMSVCRoot(int Index);
