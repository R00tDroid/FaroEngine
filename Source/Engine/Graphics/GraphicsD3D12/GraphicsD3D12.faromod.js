function configureModule(module) {
    module.setName("GraphicsD3D12");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");

    module.addIncludePrivate(module.getDirectory() + "/Thirdparty/DirectX-Headers/include");

    module.addDependency("GraphicsBase");
    module.addDependency("PlatformWindows");

    module.setSolutionLocation("Graphics");
}

function configureSetup(setup, module) {
    //TODO Link D3D12.lib and DXGI.lib
}
