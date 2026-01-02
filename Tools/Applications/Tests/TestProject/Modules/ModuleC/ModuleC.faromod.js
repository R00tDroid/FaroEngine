function configureModule(module) {
    module.setName("ModuleC");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");

    module.addDependency("ModuleA");
    module.addDependency("ModuleB");

    module.setType("executable");

    module.setSolutionLocation("Apps/Tests");
}

function configureSetup(_setup, _module) {
}
