#pragma once
#include <Primitives.hpp>
#include "String.hpp"

namespace Faro
{
    struct Version
    {
        Version(uint16 major, uint16 minor, uint16 patch, String commit, String branch = "") : major(major), minor(minor), patch(patch), commit(commit), branch(branch) {}

        uint16 major;
        uint16 minor;
        uint16 patch;
        String commit;
        String branch;

        String GetShortVersion()
        {
            return ToString(major) + "." + ToString(minor) + "." + ToString(patch);
        }

        String GetVersion()
        {
            return GetShortVersion() + "." + commit;
        }

        String GetFullVersion()
        {
            return GetVersion() + (branch.Empty() ? "" : (" (" + branch + ")"));
        }
    };
}
