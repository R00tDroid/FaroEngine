function configureModule(module) {
    module.setName("WindowCreation");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");
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
