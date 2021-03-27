#include "misc/Timer.hpp"

using hsclock = RIS::Timer::tp::clock;

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

	Timer::fseconds Timer::DeltaDuration() const
	{
		return fseconds(curTime - prevTime);
	}

	Timer::fseconds Timer::TotalDuration() const
	{
		return fseconds(curTime - startTime);
	}

	Timer::tp Timer::CurrentTime() const
	{
		return curTime;
	}
}
