#pragma once

#include <chrono>

class LGameTimer
{
public:
	static LGameTimer* Get();
	static void Release();

	void Reset();
	void SetTimeScale(float Time = 1.0f);
	float GetTimeScale();

	float GetDeltaTime();
	void Tick();

private:
	LGameTimer();
	~LGameTimer();

	static LGameTimer* TimerInstance;

	std::chrono::system_clock::time_point StartTime;
	std::chrono::duration<float> DeltaTime;
	float TimeScale;

};
