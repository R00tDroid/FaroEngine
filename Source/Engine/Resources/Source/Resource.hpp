#pragma once
#include <Primitives.hpp>
#include <ThreadSafety.hpp>

enum ResourceState
{
    RS_Unknown,
    RS_Queued,
    RS_Loading,
    RS_Available,
    RS_PendingDestroy
};

namespace Faro
{
    class IResource
    {
    public:
        void ClaimResource();
        void ReleaseResource();
        bool IsResourceClaimed();

        ResourceState GetResourceState();

    protected:
        virtual void InitResource() = 0;
        virtual void DestroyResource() = 0;

    private:
        uint32 claims = 0;
        ThreadSafe<ResourceState> state = RS_Unknown;
    };
}
