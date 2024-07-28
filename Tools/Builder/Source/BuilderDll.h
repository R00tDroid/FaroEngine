#pragma once

#ifdef FaroBuilderExport
#define BuilderExport __declspec(dllexport)
#else
#define BuilderExport __declspec(dllimport)
#endif
