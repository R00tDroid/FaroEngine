cmake_minimum_required(VERSION 3.15)
project(FaroBuildTool LANGUAGES CXX)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../../CMake/ProjectVersion.cmake)

message("Engine version: ${FaroEngineVersionFull}")


configure_file(${CMAKE_CURRENT_LIST_DIR}/../../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../../Source/Engine/Core/Source/EngineVersion.cpp @ONLY)

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(
    ${PROJECT_NAME}
    ${SourceFiles}
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2/tinyxml2.cpp
)

target_link_libraries(${PROJECT_NAME}
    Rpcrt4.lib
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-project \"${CMAKE_CURRENT_LIST_DIR}\\..\\..\\Source\\FaroEngine.faroproject.json\" -platform windows x64 -debug -generate -build") 

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/picojson
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/glob/single_include
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2
)
