file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroProjects SHARED ${SourceFiles})

target_include_directories(FaroProjects PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroProjects PUBLIC FaroTools FaroToolchain)

ModuleExports(FaroProjects ${CMAKE_CURRENT_LIST_DIR}/Source)
