# Building

## Building the toolchain
1. Navigate to the "Tools" folder and open a command prompt
2. `mkdir Build`
3. `cd Build`
4. `cmake ..`
5. `cmake --build . --config Release`
6. All tools are now in the `Bin` folder in the root of the repository

## Build the engine
1. Navigate to the "Source" folder and open a command prompt
2. `FaroBuildTool -generate -project FaroEngine.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project FaroEngine.faroproject.json -platform Windows x64 -release`


## Building a project
1. Navigate to the project folder and open a command prompt
2. `FaroBuildTool -generate -project ProjectName.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project ProjectName.faroproject.json -platform Windows x64 -release`
