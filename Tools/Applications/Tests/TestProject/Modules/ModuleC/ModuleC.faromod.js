function configureModule(module) {
    module.setName("ModuleC");

    module.scanSource(module.dir() + "/Source/**/*.cpp");
    module.scanSource(module.dir() + "/Source/**/*.hpp");
    module.scanSource(module.dir() + "/Source/**/*.c");
    module.scanSource(module.dir() + "/Source/**/*.h");

    print(module);
}

function configureSetup(setup, module) {
    print(setup.config());
    print(setup.target());

    print(module);
}
