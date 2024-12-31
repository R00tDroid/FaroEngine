file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_library(FaroCLI SHARED ${SourceFiles})

target_include_directories(FaroCLI PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroCLI PUBLIC FaroTools FaroProjects FaroGeneration)
