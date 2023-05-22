# Setting up a project

## Project manifest
A project is defined by a single file: A project manifest. This specifies some details and describes what modules are part of the project.

The manifest is structured as json data. The root element is an object that contains values to describe the project.

The following values can be set:
| Value | Type | Description |
| ---: | :---: | :--- |
| Name | String | Name of the project. |
| EngineVersion | String | Required version of the engine. Must be 3 elements: X.X.X |
| Modules | Array | List of strings to indicate where module manifests can be found relative to the project manifest. |

A typical project manifest will look like this:
```
{
    "Name": "My Game",
    "EngineVersion": "1.0.0",
    "Modules": 
    [
        "Source/MyGame",
    ]
}
```

## Adding modules
This engine is completely modular. All features are contained within their own module and can interact with others through dependencies.

There are two types of modules: Library and Application.
* Library modules define code that can be used by other modules. They are not executable on their own.
* Application modules are linked with any library dependencies into an executable.

A module manifest defines what files are to be compiled, what directories should be used for include searches and what other modules its code depends on. The manifest is structured as json data. The root element is an object that contains values to describe the module.

Adding code files to a module is done through filters. The toolchain will use these filters to look for files automatically.

Include directories control how the compiler searches for included headers. These can be private to a specific module, or be public to and dependant modules.

The following values can be set:
| Value | Type | Description |
| ---: | :---: | :--- |
| PublicIncludeDirectories | Array | List of string that indicate where directories can be found relative to the module manifest. |
| PrivateIncludeDirectories | Array | List of string that indicate where directories can be found relative to the module manifest. |
| Dependencies | Array | List of strings to indicate what modules this one depends on. |
| SourceFilters | Array | List of strings to indicate where source files can be found relative to this module manifest. |
| ModuleType | String | Linkage type of the module. Either Library or Application. |

A typical module manifest looks like this:
```
{
    "PublicIncludeDirectories": [
        "Source"
    ],
    "Dependencies": [
        "PlatformWindows",
        "GraphicsD3D12"
    ],
    "SourceFilters": [
        "Source/**/*.cpp",
        "Source/**/*.hpp",
    ],
    "ModuleType": "Application",
}
```

## Generating a Visual Studio Solution
1. Navigate to the project folder and open a command prompt
2. `FaroBuildTool -generate -project ProjectName.faroproject.json`
3. Open the generated solution file in Visual Studio

## Building the project {#ProjectSetupBuild}
1. Navigate to the project folder and open a command prompt
2. `FaroBuildTool -generate -project ProjectName.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project ProjectName.faroproject.json -platform Windows x64 -release`
