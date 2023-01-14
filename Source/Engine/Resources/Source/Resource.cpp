#include "Resource.hpp"
#include <Assert.hpp>

#include "ResourceManager.hpp"
#include "Containers/Stream.hpp"

namespace Faro
{
    IResource::IResource(DataStream* inStream) : stream(inStream) { }

    IResource::~IResource()
    {
        stream->Close();
    }

    void IResource::ClaimResource()
    {
        claims++;

        if (claims == 1)
        {
            GResources.OnResourceClaimChanged(this);
        }
    }

    void IResource::ReleaseResource()
    {
        Debug_Assert(claims > 0, "Mismatched claim and release");
        claims--;

        if (claims == 0)
        {
            GResources.OnResourceClaimChanged(this);
        }
    }

    bool IResource::IsResourceClaimed()
    {
        return claims > 0;
    }

    ResourceState IResource::GetResourceState()
    {
        return state.Get();
    }

    bool IResource::IsAvailable()
    {
        return GetResourceState() == RS_Available;
    }

    DataStream* IResource::GetDataStream()
    {
        return stream;
    }

    void IResource::NotifyNewState(ResourceState newState)
    {
        state.Set(newState);
    }
}
