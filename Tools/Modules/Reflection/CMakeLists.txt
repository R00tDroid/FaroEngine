file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroReflection SHARED ${SourceFiles})

target_include_directories(FaroReflection PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroReflection PUBLIC
    FaroTools
    FaroProjects
    FaroToolchain
    tinyxml2
    tree-sitter
    tree-sitter-cpp
)

ModuleExports(FaroReflection ${CMAKE_CURRENT_LIST_DIR}/Source)
