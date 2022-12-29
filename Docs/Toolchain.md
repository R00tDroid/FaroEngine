# Toolchain usage

## Build tool CLI
The build tool is responsible for generating and building your code. It is controlled through the commandline.

### Actions
* Generating (`-generate`): This will parse the project and module manifests and generate all information required to build the project. Additionally, this creates a Visual Studio solution.
* Building (`-build`): Builds all modules in the project and their dependencies. This will result in the creation of an executable.

### Parameters
* Project path (`-project [PathToProject]`): Specifies the project for which to perform any actions.
* Build configuration (`-debug / -development / -release`): Specifies what configuration to perform any actions for.
    * Debug: Debuggable code without optimization
    * Development: Debuggable code with optimization
    * Release: Optimized code without debug information
* Platform (`-platform [PlatformName] [Architecture]`): Specifies the target platform.

## Shader transpiler CLI
The shader transpiler is responsible for compiling and transpiling HLSL shaders. It is controlled through the commandline.

### Parameters
* Project path (`-shader [PathToShader]`): Specifies one or more shader files to queue for processing.
* Build configuration (`-include [PathToDirectory]`): Specifies one or more directories for include searches.

