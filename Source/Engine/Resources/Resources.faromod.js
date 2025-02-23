function configureModule(module) {
    module.setName("Resources");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");

    module.addDependency("PlatformBase");
}

function configureSetup(setup, module) {
}

