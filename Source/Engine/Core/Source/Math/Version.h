#ifndef VERSION_HEADER
#define VERSION_HEADER
#include "Primitives.h"
#include "Containers/String.h"

namespace Faro
{
    struct Version
    {
        Version(uint8 Major, uint8 Minor, uint8 Revision, String Commit = "", String Branch = "") : major(Major), minor(Minor), revision(Revision), commit(Commit), branch(Branch) {}

        uint8 major;
        uint8 minor;
        uint8 revision;
        String commit;
        String branch;
    };
}

#endif