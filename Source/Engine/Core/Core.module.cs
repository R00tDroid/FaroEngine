using FaroEngine;

class CoreModule : Module
{
    public CoreModule(TargetConfiguration target) : base(target)
    {
        sourcePaths.Add("Source/**/*.cpp");
        sourcePaths.Add("Source/**/*.h");
    }
}