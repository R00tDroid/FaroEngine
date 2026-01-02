function configureModule(module) {
    module.setName("GraphicsBase");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");

    module.addDependency("Core");
    module.addDependency("PlatformBase");

    module.setSolutionLocation("Graphics");
}

function configureSetup(_setup, _module) {
}
