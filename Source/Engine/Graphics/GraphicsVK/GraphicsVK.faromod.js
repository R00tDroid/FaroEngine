function configureModule(module) {
    module.setName("GraphicsVK");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");
    module.scanSource(module.getDirectory() + "/Thirdparty/GLAD/src/*.c");
    module.addIncludePrivate(module.getDirectory() + "/Thirdparty/GLAD/include");

    module.addDependency("GraphicsBase");
    module.addDependency("PlatformWindows");

    module.setSolutionLocation("Graphics");
}

function configureSetup(_modulesetup, _module) {
}
