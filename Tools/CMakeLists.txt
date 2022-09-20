cmake_minimum_required(VERSION 3.15)
project(FaroTools)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/../Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/../Bin)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BuildTool")

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)

