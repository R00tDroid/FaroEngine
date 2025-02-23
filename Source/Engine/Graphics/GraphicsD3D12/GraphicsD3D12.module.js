function configureModule(module) {
    module.setName("GraphicsD3D12");

    module.scanSource(module.getDirectory() + "/Source/**/*.cpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.hpp");
    module.scanSource(module.getDirectory() + "/Source/**/*.c");
    module.scanSource(module.getDirectory() + "/Source/**/*.h");

    module.addIncludePrivate(module.getDirectory() + "/Thirdparty/DirectX-Headers/include");

    module.addDependency("GraphicsBase");
    module.addDependency("PlatformBase");

    module.setSolutionLocation("Graphics");
}

function configureSetup(setup, module) {
    //TODO Link D3D12.lib and DXGI.lib
}
