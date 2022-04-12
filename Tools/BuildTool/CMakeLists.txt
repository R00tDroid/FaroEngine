cmake_minimum_required(VERSION 3.15)
project(FaroBuildTool LANGUAGES CSharp)

include(${CMAKE_CURRENT_LIST_DIR}/../../CMake/ProjectVersion.cmake)

message("Engine version: ${FaroEngineVersionFull}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.cs.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.cs @ONLY)

include(CSharpUtilities)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.cs
)

add_executable(
    ${PROJECT_NAME}
    ${SourceFiles} 
)

LIST(APPEND VS_DOTNET_REFERENCES "System")
LIST(APPEND VS_DOTNET_REFERENCES "System.Xml")

set_target_properties(${PROJECT_NAME} PROPERTIES
    DOTNET_TARGET_FRAMEWORK_VERSION "v4.8"
    VS_DOTNET_REFERENCES "${VS_DOTNET_REFERENCES}"
    VS_PACKAGE_REFERENCES "Microsoft.Extensions.FileSystemGlobbing_6.0.0;Newtonsoft.Json_13.0.1"
)
