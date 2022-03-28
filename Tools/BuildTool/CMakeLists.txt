cmake_minimum_required(VERSION 3.15)
project(FaroBuildTool LANGUAGES CSharp)

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
LIST(APPEND VS_DOTNET_REFERENCES "System.Memory")
LIST(APPEND VS_DOTNET_REFERENCES "System.Text.Json")

set_target_properties(${PROJECT_NAME} PROPERTIES
    DOTNET_TARGET_FRAMEWORK_VERSION "v4.8"
    VS_DOTNET_REFERENCES "${VS_DOTNET_REFERENCES}"
)
