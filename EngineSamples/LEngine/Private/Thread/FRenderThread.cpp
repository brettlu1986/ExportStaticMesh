
#include "pch.h"

#include "FRenderThread.h"
#include "FRHI.h"
#include "LLog.h"

FRenderThread* FRenderThread::RenderThread = nullptr;
bool FRenderThread::Inited = false;

FRenderThread::FRenderThread()
	:FTaskThread("RenderThread")
{

}

void FRenderThread::OnThreadInit()
{
	RenderScene = make_unique<FScene>();
	Renderer.Initialize(GetRenderScene());
	Inited = true;
}

bool FRenderThread::IsInited()
{
	return Inited;
}

void FRenderThread::Run()
{
	RHIInit();
	OnThreadInit();
	while (IsRun)
	{
		//LLog::Log("RenderTHread ::%d \n", (INT)SyncCount);

		WaitForGameThread();

		GRHI->BeginRenderScene();

		DoTasks();
		Renderer.RenderScene(GetRenderScene());
		GRHI->EndRenderScene();

		--SyncCount;
		RenderCV.notify_all();
	}
	Clear();
	RHIExit();
}

void FRenderThread::Stop()
{
	++SyncCount;
	RenderCV.notify_all();

	FThread::Stop();
}

void FRenderThread::CreateRenderThread()
{
	if (!RenderThread)
		RenderThread = new FRenderThread;
}

void FRenderThread::DestroyRenderThread()
{
	RenderThread->Stop();
	delete RenderThread;
	RenderThread = nullptr;
}

FRenderThread* FRenderThread::Get()
{
	return RenderThread;
}

void FRenderThread::Clear()
{
	DoTasks();
	Renderer.Destroy();
	RenderScene = nullptr;
}

void FRenderThread::WaitForRenderThread()
{
	//LLog::Log("GameTHread ::%d \n", (INT)SyncCount);

	//cv false will block
	unique_lock<mutex> Lock(Mutex);
	RenderCV.wait(Lock, [this]() { return SyncCount <= CPU_MAX_AHEAD; });
	++SyncCount;

	TasksIndex = (TasksIndex + 1) % FRAME_COUNT;
	RenderCV.notify_all();
}

void FRenderThread::WaitForGameThread()
{
	unique_lock<mutex> Lock(Mutex);
	RenderCV.wait(Lock, [this]() { return SyncCount > 0; });
}
