set(ExportRoot ${CMAKE_CURRENT_LIST_DIR})

function(ModuleExports Module Directory)
    set(ModuleName ${Module})
    configure_file(${ExportRoot}/Exports.h.in ${Directory}/${Module}Exports.generated.h @ONLY)
    target_compile_definitions(${Module} PRIVATE -D${Module}_Impl)
endfunction()
