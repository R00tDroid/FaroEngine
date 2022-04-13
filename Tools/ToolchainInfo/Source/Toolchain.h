#pragma once
#include "Dll.h"

DllExport int CountAvailableWindowsKits();
DllExport char* GetAvailableWindowsKitRoot(int Index);
DllExport char* GetAvailableWindowsKitVersion(int Index);
