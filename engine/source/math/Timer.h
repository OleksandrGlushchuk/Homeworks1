#pragma once
#include <chrono>
class Timer
{
public:
	std::chrono::time_point<std::chrono::steady_clock> start;
	void RestartTimer()
	{
		start = std::chrono::steady_clock::now();
	}
	bool TimeElapsed (const float& seconds)
	{
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count() >= seconds ? true : false;
	}
	float GetDeltaTime()
	{
		return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count();
	}
};