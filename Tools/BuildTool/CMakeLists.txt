cmake_minimum_required(VERSION 3.15)
project(FaroBuildTool LANGUAGES CXX)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

message("Engine version: ${FaroEngineVersionFull}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Tests/TestInfo.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Tests/TestInfo.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

macro(SetupToolTarget TargetName)
    add_executable(
        ${TargetName}
        ${SourceFiles}
        ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2/tinyxml2.cpp
    )

    if(MSVC)
        target_link_libraries(${TargetName}
            Rpcrt4.lib
        )
    else()
        target_link_libraries(${TargetName}
            uuid
        )
    endif()

    include_directories(
        ${CMAKE_CURRENT_LIST_DIR}/Source
        ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils
        ${CMAKE_CURRENT_LIST_DIR}/../Shared/WindowsKitInfo
        ${CMAKE_CURRENT_LIST_DIR}/../Shared/MSVCInfo
        ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
    )

    target_compile_features(${TargetName} PRIVATE cxx_std_17)

    target_include_directories(${TargetName} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/Dependencies/picojson
        ${CMAKE_CURRENT_LIST_DIR}/Dependencies/glob/single_include
        ${CMAKE_CURRENT_LIST_DIR}/Dependencies/tinyxml2
    )
endmacro()

SetupToolTarget(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-project \"${CMAKE_CURRENT_LIST_DIR}\\..\\..\\Source\\FaroEngine.faroproject.json\" -platform WindowsX64 -debug -generate -build")
target_compile_definitions(${PROJECT_NAME} PUBLIC FaroToolTest=0)

SetupToolTarget(${PROJECT_NAME}_Test)
target_link_libraries(${PROJECT_NAME}_Test gtest)
target_compile_definitions(${PROJECT_NAME}_Test PUBLIC FaroToolTest=1)
