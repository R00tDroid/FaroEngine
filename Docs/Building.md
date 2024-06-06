# Building

## Building the toolchain
The toolchain will be used to build the engine and your projects. You build it yourself, or you can obtain a packaged version of the engine, which has a pre-built toolchain. Follow these instructions to build the toolchain yourself:
1. Open a command prompt in the directory you want the engine to end up
2. `git clone --recursive https://github.com/R00tDroid/FaroEngine.git`
3. `cd Tools`
4. `mkdir .Build`
5. `cd .Build`
6. `cmake ..`
7. `cmake --build . --config Release`
8. All tools are now in the `Bin` folder in the engine directory

## Build the engine
Building the engine yourself is not necessary. All required modules will be compiled as part of your project's build pipeline. If you wish to build it yourself for testing or debugging purposes, follow these instructions:
1. Navigate to the "Source" folder and open a command prompt
2. `FaroBuildTool -generate -project FaroEngine.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project FaroEngine.faroproject.json -platform Windows x64 -release`

## Building a project
Follow the instructions on [Project Setup](@ref ProjectSetupBuild) to build a project.
