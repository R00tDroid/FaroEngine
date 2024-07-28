cmake_minimum_required(VERSION 3.15)
project(FaroGUI LANGUAGES CXX)

configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/ApplicationResources.rc.in ${CMAKE_CURRENT_LIST_DIR}/Source/ApplicationResources.generated.rc @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

file(COPY ${CMAKE_CURRENT_LIST_DIR}/../../Assets/Logo/Logo_32.png DESTINATION ${CMAKE_CURRENT_LIST_DIR}/Resources/img)

file(GLOB_RECURSE ResourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Resources/*.*
)

add_executable(${PROJECT_NAME} WIN32
    ${SourceFiles}
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/sciter/include/sciter-win-main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/Dependencies/sciter/include
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"${CMAKE_CURRENT_LIST_DIR}\\..\\..\\Source\\FaroEngine.faroproject.json\"") 

add_custom_target(${PROJECT_NAME}Resources
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/Dependencies/sciter/bin/windows/packfolder ${CMAKE_CURRENT_LIST_DIR}/Resources ${CMAKE_CURRENT_LIST_DIR}/Source/Resources.generated.hpp
    SOURCES ${ResourceFiles}
)

add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}Resources)

target_link_libraries(${PROJECT_NAME} Builder ShaderTranspiler)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/Dependencies/sciter/bin/windows/x64/sciter.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
