cmake_minimum_required(VERSION 3.15)
project(FaroShaderTranspiler LANGUAGES CXX)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

message("Engine version: ${FaroEngineVersionFull}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

add_executable(
    ${PROJECT_NAME}
    ${SourceFiles}
)

if(MSVC)
    target_link_libraries(${PROJECT_NAME}
        Rpcrt4.lib
    )
else()
    target_link_libraries(${PROJECT_NAME}
        uuid
    )
endif()

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
    ${CMAKE_CURRENT_LIST_DIR}/../../Source/Engine/Graphics/GraphicsBase/Source
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-shader \"${CMAKE_CURRENT_LIST_DIR}\\TestResources\\Shader.hlsl\"") 

