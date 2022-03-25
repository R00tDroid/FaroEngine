#ifndef MODULE_HEADER
#define MODULE_HEADER

#include "./Containers/Array.h"
#include "./Containers/String.h"
#include "Util/ClassRegistry.h"

namespace Faro
{
	class Module
	{
	public:
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual String GetName() = 0;
		virtual Array<String> GetRuntimeDependencies() = 0;
	};

	extern Array<Module*> GetRegisteredModules();

	DEFINE_INSTANCE_REGISTRY(Module, Faro::Module)
#define REGISTER_MODULE(MODULE) REGISTER_INSTANCE(Module, MODULE)	
}

#endif