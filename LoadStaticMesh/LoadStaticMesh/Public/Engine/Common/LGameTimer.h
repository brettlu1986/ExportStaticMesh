#pragma once

#include <chrono>

class LGameTimer
{
public:

	typedef chrono::duration<INT, std::milli> miliseconds_type;
	typedef chrono::duration<INT, std::micro> microseconds_type;
	typedef chrono::duration<float, std::ratio<1, 1>> seconds_type;

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

	chrono::system_clock::time_point GetCurrentTime()
	{
		return chrono::system_clock::now();
	}

	chrono::system_clock::time_point GetStartTime()
	{
		return StartTime;
	}

	INT64 GetCurrentTimeSeconds()
	{
		chrono::seconds TimeSec = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch());
		return TimeSec.count();
	}

	void MarkCurrentTime()
	{
		MarkTime = chrono::system_clock::now();
	}

	INT GetMarkTimeMiliSecDuration()
	{
		miliseconds_type MiliDuration = chrono::duration_cast<miliseconds_type>(chrono::system_clock::now() - MarkTime);
		return MiliDuration.count();
	}

	INT GetMarkTimeMicroSecDuration()
	{
		microseconds_type MicroDuration = chrono::duration_cast<microseconds_type>(chrono::system_clock::now() - MarkTime);
		return MicroDuration.count();
	}

	float GetMarkTimeSecDuration()
	{
		seconds_type MiliDuration = chrono::duration_cast<seconds_type>(chrono::system_clock::now() - MarkTime);
		return MiliDuration.count();
	}

private:
	LGameTimer();
	~LGameTimer();

	static LGameTimer* TimerInstance;

	chrono::system_clock::time_point StartTime;
	chrono::duration<float> DeltaTime;
	float TimeScale;

	chrono::system_clock::time_point MarkTime;

};
