cmake_minimum_required(VERSION 3.15)
project(FaroCLI LANGUAGES CXX)

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/EmscriptenInfo/*.*
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
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/EmscriptenInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
)

target_link_libraries(${PROJECT_NAME} Builder ShaderTranspiler)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-project \"${CMAKE_CURRENT_LIST_DIR}\\..\\..\\Source\\FaroEngine.faroproject.json\" -platform WindowsX64 -debug -generate -build")
