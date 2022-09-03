#include "MathTime.h"
#include <cmath>

namespace Faro
{
    TimePoint TimePoint::Now()
    {
        TimePoint p;
        p.point = std::chrono::high_resolution_clock::now();
        return p;
    }

    int16 Duration::TotalMinutes()
    {
        return std::chrono::duration_cast<std::chrono::minutes>(duration).count();
    }

    float Duration::TotalSeconds()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f;
    }

    float Duration::TotalMilliseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0f;
    }

    int32 Duration::TotalMicroseconds()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    }

    int16 Duration::Minutes()
    {
        return std::floor(TotalSeconds() / 60.0f);
    }

    int16 Duration::Seconds()
    {
        return std::floor(TotalSeconds()) - Minutes() * 60;
    }

    int16 Duration::Milliseconds()
    {
        return std::floor(TotalMilliseconds()) - (Seconds() * 1000 + Minutes() * 60 * 1000);
    }

    int32 Duration::Microseconds()
    {
        return std::floor(TotalMicroseconds()) - (Milliseconds() + Seconds() * 1000 + Minutes() * 60 * 1000) * 1000;
    }
}
