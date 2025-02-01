add_library(Duktape STATIC ${CMAKE_CURRENT_LIST_DIR}/src/duktape.c)
target_include_directories(Duktape PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
