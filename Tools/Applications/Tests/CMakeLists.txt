configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(Tests ${SourceFiles})

target_include_directories(Tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(Tests PUBLIC gtest FaroTools FaroProjects FaroGeneration)

ModuleExports(Tests ${CMAKE_CURRENT_LIST_DIR}/Source)
