#include "Time.hpp"
#include <cmath>

namespace Faro
{
    TimePoint TimePoint::Now()
    {
        TimePoint p;
        p.point = std::chrono::high_resolution_clock::now();
        return p;
    }

    int32 Duration::TotalHours() const
    {
        return std::chrono::duration_cast<std::chrono::hours>(duration).count();
    }

    int32 Duration::TotalMinutes() const
    {
        return std::chrono::duration_cast<std::chrono::minutes>(duration).count();
    }

    int64 Duration::TotalSeconds() const
    {
        return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    }

    int64 Duration::TotalMilliseconds() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    int64 Duration::TotalMicroseconds() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    int8 Duration::Hours() const
    {
        return TotalHours() % 24;
    }

    int8 Duration::Minutes() const
    {
        return TotalSeconds() % 60;
    }

    int8 Duration::Seconds() const
    {
        return TotalSeconds() % 60;
    }

    int16 Duration::Milliseconds() const
    {
        return TotalMilliseconds() % 1000;
    }

    int16 Duration::Microseconds() const
    {
        return TotalMicroseconds() % 1000;
    }
}
