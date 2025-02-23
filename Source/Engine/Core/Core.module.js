function configureModule(module) {
    module.setName("Core");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");
    module.addSource(module.getDirectory() + "/Source/EngineVersion.generated.cpp");
}

function configureSetup(setup, module) {
}
