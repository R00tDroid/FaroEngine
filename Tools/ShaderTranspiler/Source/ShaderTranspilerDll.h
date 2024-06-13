#pragma once

#ifdef FaroShaderTranspilerExport
#define ShaderTranspilerExport __declspec(dllexport)
#else
#define ShaderTranspilerExport __declspec(dllimport)
#endif
