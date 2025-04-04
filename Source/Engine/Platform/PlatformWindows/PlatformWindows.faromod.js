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
    if (setup.getTarget().includes("windows")) {
        
        module.addLinkerLibrary("user32.lib");

        if (setup.getConfig() == "release") {
            module.addLinkerLibrary("libucrt.lib");
            module.addLinkerLibrary("libvcruntime.lib");
        }
        else {
            module.addLinkerLibrary("libucrtd.lib");
            module.addLinkerLibrary("libvcruntimed.lib");
        }
    }
}

