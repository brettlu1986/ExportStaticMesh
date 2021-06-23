
#include "pch.h"
#include "FThread.h"

FThread::FThread(const string& InName)
:Name(InName)
,IsRun(false)
,IsFinished(false)
{

}

void FThread::Start()
{
	if(!IsRun)
	{
		IsRun = true;
		IsFinished = false;
		ThisThread = thread(&FThread::ThreadMain, this);
		ThisThread.detach();
	}
}	

void FThread::ThreadMain()
{
	ThreadId = this_thread::get_id();
	SetName();
	Run();
	IsFinished = true;
	CVFinished.notify_all();
}

void FThread::Stop()
{
	if(IsRun)
	{
		IsRun = false;
	}

	unique_lock<mutex> Lock(Mutex);
	CVFinished.wait(Lock, [this](){ return IsFinished == true;});
}


void FThread::SetName()
{
#if defined(_DEBUG)
	/**
		 * Code setting the thread name for use in the debugger.
		 *
		 * http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		 */
	const UINT MS_VC_EXCEPTION = 0x406D1388;

	struct THREADNAME_INFO
	{
		UINT dwType;	   // Must be 0x1000.
		LPCSTR szName;	   // Pointer to name (in user addr space).
		UINT dwThreadID; // Thread ID (-1=caller thread).
		UINT dwFlags;	   // Reserved for future use, must be zero.
	};

	THREADNAME_INFO ThreadNameInfo;
	ThreadNameInfo.dwType = 0x1000;
	ThreadNameInfo.szName = Name.c_str();
	ThreadNameInfo.dwThreadID = ::GetCurrentThreadId();
	ThreadNameInfo.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable : 6320 6322)
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(ThreadNameInfo) / sizeof(ULONG_PTR), (ULONG_PTR*)&ThreadNameInfo);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
#pragma warning(pop)
#endif
}
