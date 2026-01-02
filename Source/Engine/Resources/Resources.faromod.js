function configureModule(module) {
    module.setName("Resources");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");

    module.addDependency("PlatformBase");
}

function configureSetup(_setup, _module) {
}

