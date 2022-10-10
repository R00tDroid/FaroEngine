cmake_minimum_required(VERSION 3.15)
project(FaroTools)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../Bin")

foreach(ConfigName ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${ConfigName} ConfigName)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/../Bin" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/../Bin" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${ConfigName} "${CMAKE_CURRENT_LIST_DIR}/../Bin" CACHE PATH "" FORCE)
endforeach()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BuildTool")

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)
