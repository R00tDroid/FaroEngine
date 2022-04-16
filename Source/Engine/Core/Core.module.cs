using FaroEngine;

class CoreModule : Module
{
    public CoreModule(TargetConfiguration target) : base(target)
    {
        moduleType = ModuleType.Library;

        sourcePaths.Add("Source/**/*.cpp");
        sourcePaths.Add("Source/**/*.h");
    }
}