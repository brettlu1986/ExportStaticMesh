
#include "stdafx.h"

#include "FRenderThread.h"
#include "FRHI.h"

FRenderThread* FRenderThread::RenderThread = nullptr;
bool FRenderThread::Inited = false;

FRenderThread::FRenderThread()
	:FTaskThread("RenderThread")
	//, RenderScene(nullptr)
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
		//while(ShouldWaitGame())
		//{
		//	continue;
		//}

		GRHI->BeginRenderScene();

		DoTasks();
		
		Renderer.RenderScene(GetRenderScene());
		GRHI->EndRenderScene();

		//NotifyGameExcute();
		--SyncCount;
		RenderCV.notify_all();
	}
	Clear();
	RHIExit();
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
	RenderScene = nullptr;
	ClearTask();

	//lock_guard<mutex> Lock(Mutex);
	//SyncCount = 0;
}

void FRenderThread::WaitForRenderThread()
{
	unique_lock<mutex> Lock(Mutex);
	//false will block
	RenderCV.wait(Lock, [this]() { return SyncCount <= CPU_MAX_AHEAD; });
	++SyncCount;
}

//void FRenderThread::NotifyRenderThreadExcute()
//{
//	lock_guard<mutex> Lock(Mutex);
//	++SyncCount;
//}
//
//void FRenderThread::NotifyGameExcute()
//{
//	lock_guard<mutex> Lock(Mutex);
//	--SyncCount;
//}
//
//bool FRenderThread::ShouldWaitRender()
//{	
//	lock_guard<mutex> Lock(Mutex);
//	return SyncCount > CPU_MAX_AHEAD;
//}
//
//bool FRenderThread::ShouldWaitGame()
//{
//	lock_guard<mutex> Lock(Mutex);
//	return SyncCount <= 0;
//}
