cmake_minimum_required(VERSION 3.15)
project(Builder LANGUAGES CXX)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/EmscriptenInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

add_library(${PROJECT_NAME} SHARED ${SourceFiles} ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2/tinyxml2.cpp)

if(MSVC)
    target_link_libraries(${PROJECT_NAME}
        Rpcrt4.lib
    )
else()
    target_link_libraries(${PROJECT_NAME}
        uuid
    )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/EmscriptenInfo
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/picojson
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/glob/single_include
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2
)

target_link_libraries(${PROJECT_NAME} ShaderTranspiler)

target_compile_definitions(${PROJECT_NAME} PRIVATE FaroBuilderExport)
