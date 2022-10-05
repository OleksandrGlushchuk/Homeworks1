#pragma once
#include <chrono>
class Timer
{
public:
	std::chrono::time_point<std::chrono::steady_clock> start;
	void StartTimer()
	{
		start = std::chrono::steady_clock::now();
	}
	bool TimeElapsed (const float& seconds)
	{
		if (std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count() >= seconds)
		{
			start = std::chrono::steady_clock::now();
			return true;
		}
		return false;
	}
};