#pragma once

#include <chrono>

namespace RIS
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		void Update();
		float Delta() const;
		float Total() const;

	private:
		using tp = std::chrono::time_point<std::chrono::high_resolution_clock>;

		tp startTime, prevTime, curTime;

	};
}
