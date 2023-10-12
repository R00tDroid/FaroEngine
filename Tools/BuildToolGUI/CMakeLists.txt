cmake_minimum_required(VERSION 3.15)
project(FaroBuildToolGUI LANGUAGES CXX)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

message("Engine version: ${FaroEngineVersionFull}")

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/ApplicationResources.rc.in ${CMAKE_CURRENT_LIST_DIR}/Source/ApplicationResources.generated.rc @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/../../Assets/Logo/Logo_32.png DESTINATION ${CMAKE_CURRENT_LIST_DIR}/Resources/img)

file(GLOB_RECURSE NuklearFiles
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/Nuklear/src/*.c
)

add_executable(${PROJECT_NAME} WIN32
    ${SourceFiles}
    ${NuklearFiles}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/Nuklear/src
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"${CMAKE_CURRENT_LIST_DIR}\\..\\..\\Source\\FaroEngine.faroproject.json\"") 
