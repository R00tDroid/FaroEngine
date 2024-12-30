file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroGeneration SHARED ${SourceFiles})

target_include_directories(FaroGeneration PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroGeneration PUBLIC FaroTools FaroProjects)

ModuleExports(FaroGeneration ${CMAKE_CURRENT_LIST_DIR}/Source)
