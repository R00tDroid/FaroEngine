file(GLOB_RECURSE SourceFiles
    ${CMAKE_CURRENT_LIST_DIR}/Source/*.*
    ${CMAKE_CURRENT_LIST_DIR}/../../Shared/MSVCInfo/*.*
)

add_library(FaroGeneration SHARED ${SourceFiles})

target_include_directories(FaroGeneration PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Source)
target_include_directories(FaroGeneration PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../Shared/MSVCInfo)

target_link_libraries(FaroGeneration PUBLIC FaroTools FaroProjects FaroToolchain tinyxml2)

ModuleExports(FaroGeneration ${CMAKE_CURRENT_LIST_DIR}/Source)
