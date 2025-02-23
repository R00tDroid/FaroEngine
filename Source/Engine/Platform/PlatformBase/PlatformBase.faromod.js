function configureModule(module) {
    module.setName("PlatformBase");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");

    module.addDependency("Core");

    module.setSolutionLocation("Platform");
}

function configureSetup(setup, module) {
}
