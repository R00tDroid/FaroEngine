#ifndef MATHTIME_HEADER
#define MATHTIME_HEADER

#include <chrono>
#include "../Primitives.h"

namespace Faro{

	struct RunTime {
		long long Frame;
		long long Millis;
		float Delta;
		int Fps;
	};

	class TimeSpan
	{
	public:
		TimeSpan(std::chrono::steady_clock::duration duration) : duration(duration){}

		int16 TotalMinutes();
		float TotalSeconds();
		float TotalMilliseconds();
		int32 TotalMicroseconds();

		int16 Minutes();
		int16 Seconds();
		int16 Milliseconds();
		int32 Microseconds();
	private:
		std::chrono::steady_clock::duration duration;
	};

	class TimePoint
	{
	public:
		static TimePoint Now();

		TimeSpan operator- (TimePoint& other) const { return point - other.point; }

	private:
		std::chrono::steady_clock::time_point point;
	};	
}

#endif