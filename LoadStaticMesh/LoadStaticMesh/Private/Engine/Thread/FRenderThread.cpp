#include "FRenderThread.h"
#include "FRHI.h"

FRenderThread* FRenderThread::RenderThread = nullptr;

FRenderThread::FRenderThread()
	:FTaskThread("RenderThread")
	, FrameIndex(0)
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

		GRHI->RHIUpdateFrameResource(FrameIndex);
		GRHI->RHIBeginRenderFrame(FrameIndex);
		FFrameResource& FrameResource = GRHI->RHIGetFrameResource(FrameIndex);
		Renderer.RenderFrameResource(FrameResource);
		GRHI->RHIEndRenderFrame(FrameIndex);

		FrameIndex = (FrameIndex + 1) % FRAME_COUNT;

		NotifyGameExcute();
		//--FrameTaskNum;
		//RenderCV.notify_all();
	}
	ClearCounter();
	ClearTask();
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
		GRHI->RHICreateFrameResources(Scene);
	});
}

void FRenderThread::UpdateFrameCamera(LCamera& Camera)
{
	AddTask([this, &Camera] {
		GRHI->RHIUpdateFrameResourceCamera(Camera);
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

void FRenderThread::ClearCounter()
{
	lock_guard<mutex> Lock(Mutex);
	SyncCount = 0;
}

//void FRenderThread::WaitForRenderThread()
//{
//	unique_lock<mutex> Lock(Mutex);
//	RenderCV.wait(Lock, [this]() { return FrameTaskNum <= FRAME_COUNT; });
//	++FrameTaskNum;
//}
//void FRenderThread::UpdateRenderThreadScene(FScene* Scene)
//{
//	AddTask([this, Scene]{
//		UpdateFrameResources(Scene, FrameIndex);
//	});
//}
//
//void FRenderThread::DrawThreadThreadScene(FScene* Scene)
//{
//	AddTask([this, Scene] {
//
//		BeginRenderFrame(FrameIndex);
//		FFrameResource& FrameResource = GetFrameResource(FrameIndex);
//		Renderer.RenderFrameResource(FrameResource);
//		EndRenderFrame(FrameIndex);
//
//		FrameIndex = (FrameIndex + 1) % FRAME_COUNT;
//		--FrameTaskNum;
//		RenderCV.notify_all();
//	});
//}