cmake_minimum_required(VERSION 3.15)
project(FaroTools)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ModuleExports.cmake)
string(REPLACE "." "," FaroEngineVersionShortComma ${FaroEngineVersionShort})
message("Engine version: ${FaroEngineVersionFull}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_LIST_DIR}/../Bin")

foreach(ConfigName ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${ConfigName} ConfigName)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${ConfigName} "${EXECUTABLE_OUTPUT_PATH}" CACHE PATH "" FORCE)
endforeach()

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/Shared/picojson
)
AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/glob")
AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/tinyxml2")
#AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/googletest")

AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Tools")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Project")
#AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Generation")
#AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Shaders")
#AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Building")
#AddModule("${CMAKE_CURRENT_LIST_DIR}/Applications/FaroCLI")

#if(MSVC)
#    AddModule("${CMAKE_CURRENT_LIST_DIR}/Applications/FaroGUI")
#endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)
