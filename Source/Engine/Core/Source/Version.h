#ifndef VERSION_HEADER
#define VERSION_HEADER
#include "Primitives.h"

namespace Faro
{
	struct Version
	{
		Version(uint8 Major, uint8 Minor, uint8 Revision) : Major(Major), Minor(Minor), Revision(Revision){}

		uint8 Major;
		uint8 Minor;
		uint8 Revision;
	};

	static Version EngineVersion(3, 0, 0);
}

#endif