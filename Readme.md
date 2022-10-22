# Faro Engine

![Logo](./Assets/Logo/Logo_128.png)

A modular platform agnostic engine designed for low-overhead graphics.

---

[![FaroEngine Package Engine](https://github.com/R00tDroid/FaroEngine/actions/workflows/build.yml/badge.svg)](https://github.com/R00tDroid/FaroEngine/actions/workflows/build.yml)
[![pages-build-deployment](https://github.com/R00tDroid/FaroEngine/actions/workflows/pages/pages-build-deployment/badge.svg)](https://github.com/R00tDroid/FaroEngine/actions/workflows/pages/pages-build-deployment)
[![CodeFactor](https://www.codefactor.io/repository/github/r00tdroid/faroengine/badge)](https://www.codefactor.io/repository/github/r00tdroid/faroengine)

[Online documentation](https://r00tdroid.github.io/FaroEngine)

## How to build

### Build the toolchain
1. Navigate to the "Tools" folder and open a command prompt
2. `mkdir Build`
3. `cd Build`
4. `cmake ..`
5. `cmake --build . --config Release`
6. All tools are now in the `Bin` folder in the root of the repository

### Build the engine
1. Navigate to the "Source" folder and open a command prompt
2. `FaroBuildTool -generate -project FaroEngine.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project FaroEngine.faroproject.json -platform Windows x64 -release`

### Build a project
1. Navigate to the project folder and open a command prompt
2. `FaroBuildTool -generate -project ProjectName.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project ProjectName.faroproject.json -platform Windows x64 -release`
