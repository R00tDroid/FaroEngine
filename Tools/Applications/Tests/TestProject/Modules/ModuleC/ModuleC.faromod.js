function configureModule(module) {
    module.setName("ModuleC");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");

    module.addDependency("ModuleB");
    module.addDependency("ModuleC");

    module.setType("executable");
}

function configureSetup(setup, module) {
}
