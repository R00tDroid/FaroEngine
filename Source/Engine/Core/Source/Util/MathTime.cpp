#include "MathTime.h"
#include <cmath>

namespace Faro {
	TimePoint TimePoint::Now()
	{
		TimePoint p;
		p.point = std::chrono::high_resolution_clock::now();
		return p;
	}

	int16 TimeSpan::TotalMinutes()
	{
		return std::chrono::duration_cast<std::chrono::minutes>(duration).count();
	}

	float TimeSpan::TotalSeconds()
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f;
	}

	float TimeSpan::TotalMilliseconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0f;
	}

	int32 TimeSpan::TotalMicroseconds()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	}

	int16 TimeSpan::Minutes()
	{
		return std::floor(TotalSeconds() / 60.0f);
	}

	int16 TimeSpan::Seconds()
	{
		return std::floor(TotalSeconds()) - Minutes() * 60;
	}

	int16 TimeSpan::Milliseconds()
	{
		return std::floor(TotalMilliseconds()) - (Seconds() * 1000 + Minutes() * 60 * 1000);
	}

	int32 TimeSpan::Microseconds()
	{
		return std::floor(TotalMicroseconds()) - (Milliseconds() + Seconds() * 1000 + Minutes() * 60 * 1000) * 1000;
	}
}
