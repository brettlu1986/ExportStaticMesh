

#include "LGameTimer.h"

LGameTimer* LGameTimer::TimerInstance = nullptr;

LGameTimer* LGameTimer::Get()
{
	if(TimerInstance == nullptr)
		TimerInstance = new LGameTimer();

	return TimerInstance;
}

void LGameTimer::Release()
{
	delete TimerInstance;
	TimerInstance = nullptr;
}

LGameTimer::LGameTimer()
{
	Reset();
	TimeScale = 1.0f;
	DeltaTime = std::chrono::duration<float>(0.f);
}

LGameTimer::~LGameTimer()
{

}

void LGameTimer::Reset()
{
	StartTime = std::chrono::system_clock::now();
}

float LGameTimer::GetDeltaTime()
{
	return DeltaTime.count();
}

void LGameTimer::SetTimeScale(float T)
{
	TimeScale = T;
}

float LGameTimer::GetTimeScale()
{
	return TimeScale; 
}

void LGameTimer::Tick()
{
	DeltaTime = std::chrono::system_clock::now() - StartTime;

	float value = DeltaTime.count();
}