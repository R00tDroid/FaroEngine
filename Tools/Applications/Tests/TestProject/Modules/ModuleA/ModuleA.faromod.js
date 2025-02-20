function configureModule(module) {
    print(module);

    module.setName("ModuleA");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");
}

function configureSetup(setup, module) {
    print(module);
    print(setup.getTarget() + "/" + setup.getConfig());
}
