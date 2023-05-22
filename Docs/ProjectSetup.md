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
uyi

## Generating a Visual Studio Solution
uyi

## Building the project {#ProjectSetupBuild}
1. Navigate to the project folder and open a command prompt
2. `FaroBuildTool -generate -project ProjectName.faroproject.json`
3. Follow either of the following methods:
    1. Open the generated solution file in Visual Studio and perform a build
    2. `FaroBuildTool -build -project ProjectName.faroproject.json -platform Windows x64 -release`
