#include "misc/Timer.hpp"

using hsclock = std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

namespace RIS
{
	Timer::Timer()
		: startTime(), prevTime(), curTime()
	{
		startTime = hsclock::now();
		prevTime = curTime = startTime;
	}

	void Timer::Update()
	{
		prevTime = curTime;
		curTime = hsclock::now();
	}

	float Timer::Delta() const
	{
		return fseconds(curTime - prevTime).count();
	}

	float Timer::Total() const
	{
		return fseconds(curTime - startTime).count();
	}
}
