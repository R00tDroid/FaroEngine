function configureModule(module) {
    module.setName("WindowCreation");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");
    module.scanSource(module.getDirectory() + "/Source/**/*.hlsl");
    module.scanSource(module.getDirectory() + "/Source/**/*.hlsli");

    module.addDependency("PlatformWindows");
    module.addDependency("PlatformWeb");
    module.addDependency("PlatformAndroid");
    module.addDependency("GraphicsD3D12");
    module.addDependency("Resources");

    module.setType("executable");

    module.setSolutionLocation("Tests");
}

function configureSetup(setup, module) {
}
