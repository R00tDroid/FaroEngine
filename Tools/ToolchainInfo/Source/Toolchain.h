#pragma once
#include "Dll.h"

DllExport int CountAvailableWindowsKits();
DllExport void GetAvailableWindowsKit(int Index, char** Root, char** Version);
