function configureModule(module) {
    print(module);

    module.setName("ModuleA");

    module.scanSource(module.dir() + "/Source/**/*.cpp");
    module.scanSource(module.dir() + "/Source/**/*.hpp");
    module.scanSource(module.dir() + "/Source/**/*.c");
    module.scanSource(module.dir() + "/Source/**/*.h");

    var source = module.source();
    print(source);
}

function configureSetup(setup, module) {
    print(module);

    print(setup.config());
    print(setup.target());
}
