function configureModule(module) {
    module.setName("Core");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");
    module.addSource(source + "/EngineVersion.generated.cpp");
}

function configureSetup(_setup, _module) {
}
