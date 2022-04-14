cmake_minimum_required(VERSION 3.15)
project(FaroBuildTool LANGUAGES CXX)

include(${CMAKE_CURRENT_LIST_DIR}/../../CMake/ProjectVersion.cmake)

message("Engine version: ${FaroEngineVersionFull}")

file(GLOB_RECURSE ToolchainSourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.h
)

add_library(ToolchainInfo SHARED
    ${ToolchainSourceFiles}
)

target_compile_features(ToolchainInfo PRIVATE cxx_std_17)
