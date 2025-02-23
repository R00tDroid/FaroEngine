file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroBuilding SHARED ${SourceFiles})

target_include_directories(FaroBuilding PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroBuilding PUBLIC FaroTools FaroProjects FaroToolchain)

ModuleExports(FaroBuilding ${CMAKE_CURRENT_LIST_DIR}/Source)