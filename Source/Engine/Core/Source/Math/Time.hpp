#pragma once
#include <chrono>
#include "../Primitives.hpp"

namespace Faro
{
    class Duration
    {
    public:
        static Duration FromHours(int16 hours) { return Duration(std::chrono::system_clock::duration(std::chrono::hours(hours))); }
        static Duration FromMinutes(int16 minutes) { return Duration(std::chrono::system_clock::duration(std::chrono::minutes(minutes))); }
        static Duration FromSeconds(int16 seconds) { return Duration(std::chrono::system_clock::duration(std::chrono::seconds(seconds))); }
        static Duration FromMilliseconds(int16 milliseconds) { return Duration(std::chrono::system_clock::duration(std::chrono::milliseconds(milliseconds))); }
        static Duration FromMicroseconds(int16 microseconds) { return Duration(std::chrono::system_clock::duration(std::chrono::microseconds(microseconds))); }

        Duration(std::chrono::steady_clock::duration duration) : duration(duration){}

        int32 TotalHours() const;
        int32 TotalMinutes() const;
        int64 TotalSeconds() const;
        int64 TotalMilliseconds() const;
        int64 TotalMicroseconds() const;

        int8 Hours() const;
        int8 Minutes() const;
        int8 Seconds() const;
        int16 Milliseconds() const;
        int16 Microseconds() const;

    private:
        std::chrono::steady_clock::duration duration;
    };

    class TimePoint
    {
    public:
        static TimePoint Now();

        Duration operator- (TimePoint& other) const { return point - other.point; }

    private:
        std::chrono::steady_clock::time_point point;
    };    
}
