function configureModule(module) {
    module.setName("PlatformWindows");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");

    module.addDependency("PlatformBase");

    module.setSolutionLocation("Platform");
}

function configureSetup(setup, module) {
}

