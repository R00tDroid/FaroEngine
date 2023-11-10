#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef _WIN32
#include <Windows.h>
#endif

#undef CreateWindow
#undef CreateWindowA
#undef CreateWindowW
