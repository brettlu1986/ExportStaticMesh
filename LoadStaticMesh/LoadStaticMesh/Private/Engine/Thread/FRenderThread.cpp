
#include "stdafx.h"

#include "FRenderThread.h"
#include "FRHI.h"

FRenderThread* FRenderThread::RenderThread = nullptr;

FRenderThread::FRenderThread()
	:FTaskThread("RenderThread")
	, RenderScene(nullptr)
{

}

void FRenderThread::Run()
{
	RHIInit();
	while (IsRun)
	{
		while(ShouldWaitGame())
		{
			continue;
		}

		DoTasks();

		GRHI->UpdateSceneResources(RenderScene);
		GRHI->BeginRenderScene();
		Renderer.RenderScene(RenderScene);
		GRHI->EndRenderScene();

		NotifyGameExcute();
		//--FrameTaskNum;
		//RenderCV.notify_all();
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

void FRenderThread::InitRenderThreadScene(FScene* Scene)
{
	AddTask([this, Scene] {
		RenderScene = Scene;
		GRHI->BeginCreateSceneResource();
		GRHI->CreateSceneResources(RenderScene);
		GRHI->EndCreateSceneResource();
	});
}

void FRenderThread::NotifyRenderThreadExcute()
{
	lock_guard<mutex> Lock(Mutex);
	++SyncCount;
}

void FRenderThread::NotifyGameExcute()
{
	lock_guard<mutex> Lock(Mutex);
	--SyncCount;
}

bool FRenderThread::ShouldWaitRender()
{	
	lock_guard<mutex> Lock(Mutex);
	return SyncCount > CPU_MAX_AHEAD;
}

bool FRenderThread::ShouldWaitGame()
{
	lock_guard<mutex> Lock(Mutex);
	return SyncCount <= 0;
}

void FRenderThread::Clear()
{
	RenderScene->Destroy();
	RenderScene = nullptr;

	ClearTask();

	lock_guard<mutex> Lock(Mutex);
	SyncCount = 0;
}

//void FRenderThread::WaitForRenderThread()
//{
//	unique_lock<mutex> Lock(Mutex);
//	RenderCV.wait(Lock, [this]() { return FrameTaskNum <= FRAME_COUNT; });
//	++FrameTaskNum;
//}

