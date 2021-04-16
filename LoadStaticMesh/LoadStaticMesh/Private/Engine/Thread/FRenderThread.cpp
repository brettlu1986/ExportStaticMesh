#include "FRenderThread.h"
#include "FRHI.h"

FRenderThread* FRenderThread::RenderThread = nullptr;



FRenderThread::FRenderThread()
:FTaskThread("RenderThread")
,FrameIndex(0)
{

}

void FRenderThread::Run()
{
	RHIInit();
	FTaskThread::Run();
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

void FRenderThread::WaitForRenderThread()
{
	unique_lock<mutex> Lock(Mutex);
	RenderCV.wait(Lock, [this]() { return FrameTaskNum <= FRAME_COUNT;});
	++FrameTaskNum;
}

void FRenderThread::InitRenderThreadScene(FScene* Scene)
{
	AddTask([this, Scene] {
		CreateFrameResources(Scene);
	});
}

void FRenderThread::UpdateRenderThreadScene(FScene* Scene)
{
	AddTask([this, Scene]{
		UpdateFrameResources(Scene, FrameIndex);
	});
}

void FRenderThread::DrawThreadThreadScene(FScene* Scene)
{
	AddTask([this, Scene] {

		BeginRenderFrame(FrameIndex);
		FFrameResource& FrameResource = GetFrameResource(FrameIndex);
		Renderer.RenderFrameResource(FrameResource);
		PresentToScreen(FrameIndex);

		FrameIndex = (FrameIndex + 1) % FRAME_COUNT;
		--FrameTaskNum;
		RenderCV.notify_all();
	});
}