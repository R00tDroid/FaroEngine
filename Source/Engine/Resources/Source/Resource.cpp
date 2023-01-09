#include "Resource.hpp"
#include <Assert.hpp>

namespace Faro
{
    void IResource::ClaimResource()
    {
        claims++;
    }

    void IResource::ReleaseResource()
    {
        Debug_Assert(claims > 0, "Mismatched claim and release");
        claims--;
    }

    bool IResource::IsResourceClaimed()
    {
        return claims > 0;
    }

    ResourceState IResource::GetResourceState()
    {
        return state.Get();
    }
}
