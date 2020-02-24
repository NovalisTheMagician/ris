#include "Timer.hpp"

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

	Timer::~Timer()
	{

	}

	void Timer::Update()
	{
		prevTime = curTime;
		curTime = hsclock::now();
	}

	float Timer::Delta() const
	{
		auto v = duration_cast<microseconds>(curTime - prevTime).count();
		return static_cast<float>(v / (1000.0f * 1000.0f));
	}

	float Timer::Total() const
	{
		auto v = duration_cast<microseconds>(curTime - startTime).count();
		return static_cast<float>(v / (1000.0f * 1000.0f));
	}
}
