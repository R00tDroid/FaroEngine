function configureModule(module) {
    print(module);

    module.setName("ModuleA");

    var source = module.getDirectory() + "/Source";
    module.addIncludePublic(source)

    module.scanSource(source + "/**/*.cpp");
    module.scanSource(source + "/**/*.hpp");
    module.scanSource(source + "/**/*.c");
    module.scanSource(source + "/**/*.h");
}

function configureSetup(setup, module) {
    print(module);
    print(setup.getTarget() + "/" + setup.getConfig());
}
