#pragma once

#include <chrono>

namespace RIS
{
	class Timer
	{
	public:
		Timer();

		void Update();
		float Delta() const;
		float Total() const;

	private:
		using tp = std::chrono::time_point<std::chrono::steady_clock>;
		using fseconds = std::chrono::duration<float>;

		tp startTime, prevTime, curTime;

	};
}
