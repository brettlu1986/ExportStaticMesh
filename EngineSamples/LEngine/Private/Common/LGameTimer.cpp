
#include "pch.h"
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
	DeltaTime = chrono::duration<float>(0.f);
}

LGameTimer::~LGameTimer()
{

}

void LGameTimer::Reset()
{
	StartTime = chrono::system_clock::now();
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
	DeltaTime = chrono::system_clock::now() - StartTime;
}

// control limit frame rate, see the url below
// https://stackoverflow.com/questions/46258641/using-stdchrono-library-to-adjust-the-application-fps-but-getting-weird-behavi
//#include <chrono> /// to use chrono namespace
//#include <iostream> /// for console output
//#include <thread> /// for this_thread::sleep_for()
//#include <windows.h>
//#define TARGET_FPS 500 /// our target fps as a macro
//const float target_fps = (float)TARGET_FPS; /// our target fps
//float tmp_target_fps = target_fps;  /// used to adjust the target fps depending on the actual real fps to reach the real target fps
//using frame_len_type = chrono::duration<float, ratio<1, TARGET_FPS>>; /// this is the     duration that defines the length of a frame
//using fsecond = chrono::duration<float>; /// this duration    represents once second and uses 'float' type as internal representation
//fsecond target_frame_len(1.0f / tmp_target_fps); /// we will define this    constant here , to represent on frame duration ( defined to avoid    construction inside a loop )
//bool enable_fps_oscillation = true;
//void app_logic()
//{
//	/** ... All application logic goes here ... **/
//}
//class HeighResolutionClockKeeper
//{
//private:
//	bool using_higher_res_timer;
//public:
//	HeighResolutionClockKeeper() : using_higher_res_timer(false) {}
//	void QueryHeighResolutionClock()
//	{
//		if (timeBeginPeriod(1) != TIMERR_NOCANDO)
//		{
//			using_higher_res_timer = true;
//		}
//	}
//	void FreeHeighResolutionClock()
//	{
//		if (using_higher_res_timer)
//		{
//			timeEndPeriod(1);
//		}
//	}
//	~HeighResolutionClockKeeper()
//	{
//		FreeHeighResolutionClock(); /// if exception is thrown , if not this wont cause problems thanks to the flag we put
//	}
//};
//int main() /// our main function !
//{
//	HeighResolutionClockKeeper MyHeighResolutionClockKeeper;
//	MyHeighResolutionClockKeeper.QueryHeighResolutionClock();
//	using sys_clock = chrono::system_clock; /// simplify the type    name to make the code readable
//	sys_clock::time_point frame_begin, frame_end; /// we will use these time points to point to frame begin and end
//	sys_clock::time_point start_point = sys_clock::now();
//	float accum_fps = 0.0f;
//	int frames_count = 0;
//	while (true)
//	{
//		frame_begin = sys_clock::now(); /// there we go !
//		app_logic(); /// lets be logical here :)
//		frame_end = sys_clock::now(); /// we are done so quick !
//		this_thread::sleep_for(target_frame_len - (frame_end.time_since_epoch() - frame_begin.time_since_epoch())); /// we will take a rest that is equal to what we where supposed to take to finish the actual target frame length
//		float fps = fsecond(1) / (sys_clock::now() - frame_begin); /// this will show ass the current FPS
//
///// obviously we will not be able to hit the exact FPS  we want se we need to oscillate around until we
///// get a very close average FPS by time .
//		if (fps < target_fps) /// our real fps is less than what we want
//			tmp_target_fps += 0.01; /// lets ask for more !
//		else if (fps > target_fps) /// it is more than what we want
//			tmp_target_fps -= 0.01; /// lets ask for less
//		if (enable_fps_oscillation == true)
//		{
//			/// now we will adjust our target frame length for match the new target FPS
//			target_frame_len = fsecond(1.0f / tmp_target_fps);
//			/// used to calculate average FPS
//			accum_fps += fps;
//			frames_count++;
//			/// each 1 second
//			if ((sys_clock::now() - start_point) > fsecond(1.0f)) /// show average each 1 sec
//			{
//				start_point = sys_clock::now();
//				cout << accum_fps / frames_count << endl; /// it is getting more close each time to our target FPS
//			}
//		}
//		else
//		{
//			/// each frame
//			cout << fps << endl;
//		}
//	}
//	MyHeighResolutionClockKeeper.FreeHeighResolutionClock();
//	return 0; /// return to OS
//} /// end of code