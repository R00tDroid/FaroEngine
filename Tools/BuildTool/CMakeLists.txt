project(FaroBuildTool LANGUAGES CXX CSharp)
cmake_minimum_required(VERSION 3.15)

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
    VS_DOTNET_REFERENCES "${VS_DOTNET_REFERENCES}"
)

set_property(TARGET ${PROJECT_NAME}
    PROPERTY VS_PACKAGE_REFERENCES "Newtonsoft.Json_13.0.1"
)
