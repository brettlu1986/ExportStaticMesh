#pragma once

#include "stdafx.h"
#include <thread>
#include <deque>
#include <mutex>

class FThread
{
public: 
	FThread(const string& InName);
	virtual ~FThread() = default;

	void ThreadMain();
	virtual void Start();
	virtual void Stop();
	virtual void Run() = 0;
	void SetName();

	thread::id GetThreadId()
	{
		return ThreadId;
	}
protected:

	string Name;
	atomic_bool IsRun;
	atomic_bool IsFinished;
	mutex Mutex;
	thread ThisThread;
	condition_variable CVFinished;
	thread::id ThreadId;

};