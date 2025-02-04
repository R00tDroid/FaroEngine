function configure(setup, module) {
    print(setup.config());
    print(setup.target());

    module.setName("ModuleA");
    module.scanSource(module.dir() + "/Source/**/*.cpp");
    module.scanSource(module.dir() + "/Source/**/*.hpp");
    module.scanSource(module.dir() + "/Source/**/*.c");
    module.scanSource(module.dir() + "/Source/**/*.h");
}
