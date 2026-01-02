configure_file(${CMAKE_CURRENT_LIST_DIR}/Source/Version.hpp.in ${CMAKE_CURRENT_LIST_DIR}/Source/Version.generated.hpp @ONLY)

file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
)

add_executable(FaroToolsTest ${SourceFiles})

target_include_directories(FaroToolsTest PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)

target_link_libraries(FaroToolsTest PUBLIC gtest FaroTools FaroProjects FaroGeneration)

ModuleExports(FaroToolsTest ${CMAKE_CURRENT_LIST_DIR}/Source)
