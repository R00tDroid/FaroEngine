function configureModule(module) {
    module.setName("ModuleB");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");
    
    module.addDependency("ModuleA");
}

function configureSetup(setup, module) {
}
