cmake_minimum_required(VERSION 3.15)
project(FaroTools)

include(${CMAKE_CURRENT_LIST_DIR}/../CMake/ProjectVersion.cmake)

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
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BuildTool")

if(MSVC)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/BuildToolGUI")
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.cpp.in ${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Core/Source/EngineVersion.generated.cpp @ONLY)

find_package(Python REQUIRED)

set(GladDirectory "${CMAKE_CURRENT_LIST_DIR}/../Source/Engine/Graphics/GraphicsVK/Thirdparty/GLAD")

file(REMOVE_RECURSE "${GladDirectory}")

execute_process(COMMAND ${Python_EXECUTABLE} -m pip install glad2)
execute_process(COMMAND ${Python_EXECUTABLE} -m glad
    --api=vulkan=1.0
    --extension=VK_KHR_surface,VK_KHR_win32_surface
    --out-path "${GladDirectory}" 
    c
    --loader)

