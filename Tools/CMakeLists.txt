cmake_minimum_required(VERSION 3.15)
project(FaroTools)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
set(tinyxml2_BUILD_TESTING FALSE)
AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/tinyxml2")
set(BUILD_GMOCK FALSE)
set(INSTALL_GTEST FALSE)
AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/googletest")
AddDependency("${CMAKE_CURRENT_LIST_DIR}/Shared/duktape")

AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Tools")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Toolchain")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Project")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Generation")
#AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Shaders") #TODO Port shader compiler
AddModule("${CMAKE_CURRENT_LIST_DIR}/Modules/Building")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Applications/FaroCLI")
AddModule("${CMAKE_CURRENT_LIST_DIR}/Applications/Tests")

#if(MSVC)
#    AddModule("${CMAKE_CURRENT_LIST_DIR}/Applications/FaroGUI") #TODO Reimplement shader compiler
#endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)

find_package(Python REQUIRED)

set(GladDirectory "${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Graphics/GraphicsVK/Thirdparty/GLAD")

file(REMOVE_RECURSE "${GladDirectory}")

execute_process(COMMAND ${Python_EXECUTABLE} -m pip install glad2)
execute_process(COMMAND ${Python_EXECUTABLE} -m glad
    --api=vulkan=1.0
    --extension=VK_KHR_surface,VK_KHR_win32_surface,VK_KHR_swapchain,VK_EXT_debug_utils
    --out-path "${GladDirectory}" 
    c
    --loader)

