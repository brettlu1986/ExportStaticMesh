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

	chrono::duration<float> GetChronoDeltaTime()
	{
		return DeltaTime;
	}

	float GetDeltaTime();
	void Tick();

private:
	LGameTimer();
	~LGameTimer();

	static LGameTimer* TimerInstance;

	chrono::system_clock::time_point StartTime;
	chrono::duration<float> DeltaTime;
	float TimeScale;

};
