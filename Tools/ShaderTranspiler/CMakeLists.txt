cmake_minimum_required(VERSION 3.15)
project(ShaderTranspiler LANGUAGES CXX)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Utils/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command/*.*
)

add_library(${PROJECT_NAME} SHARED ${SourceFiles})

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
    ${CMAKE_CURRENT_LIST_DIR}/../Shared/Command
    ${CMAKE_CURRENT_LIST_DIR}/../../Source/Engine/Graphics/GraphicsBase/Source
)

target_compile_definitions(${PROJECT_NAME} PRIVATE FaroShaderTranspilerExport)
