#include "Resource.hpp"
#include <Assert.hpp>

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

    DataStream* IResource::GetDataStream()
    {
        return stream;
    }
}
