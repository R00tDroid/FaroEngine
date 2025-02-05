file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroProjects SHARED ${SourceFiles})

target_include_directories(FaroProjects PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/Source
    ${CMAKE_CURRENT_LIST_DIR}/../../Shared/glob/single_include
)

target_link_libraries(FaroProjects PUBLIC Duktape FaroTools FaroToolchain)

ModuleExports(FaroProjects ${CMAKE_CURRENT_LIST_DIR}/Source)
