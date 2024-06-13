cmake_minimum_required(VERSION 3.15)
project(FaroTools)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)

message("Engine version: ${FaroEngineVersionFull}")

string(REPLACE "." "," FaroEngineVersionShortComma ${FaroEngineVersionShort})

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../Bin")

foreach(ConfigName ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${ConfigName} ConfigName)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
endforeach()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Shared/googletest")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/ShaderTranspiler")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Builder")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FaroCLI")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Test")

if(MSVC)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FaroGUI")
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)
