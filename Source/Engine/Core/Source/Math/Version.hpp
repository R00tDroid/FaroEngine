#pragma once
#include "Primitives.hpp"
#include "Containers/String.hpp"

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
