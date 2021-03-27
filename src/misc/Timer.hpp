#pragma once

#include <chrono>

namespace RIS
{
	class Timer
	{
	public:
		using tp = std::chrono::time_point<std::chrono::steady_clock>;
		using fseconds = std::chrono::duration<float>;

		Timer();

		void Update();
		float Delta() const;
		float Total() const;

		fseconds DeltaDuration() const;
		fseconds TotalDuration() const;
		tp CurrentTime() const;

	private:
		tp startTime, prevTime, curTime;

	};
}
