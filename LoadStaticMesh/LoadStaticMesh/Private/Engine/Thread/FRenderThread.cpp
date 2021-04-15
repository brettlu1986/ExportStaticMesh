#include "FRenderThread.h"
#include "FRHI.h"

FRenderThread* FRenderThread::RenderThread = nullptr;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
static const int FrameCount = 3;

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
	RenderCV.wait(Lock, [this]() { return FrameTaskNum <= FrameCount;});
	++FrameTaskNum;
}

void FRenderThread::InitRenderThreadScene(FScene* Scene)
{
	AddTask([this, Scene] {
		Renderer.RenderInit(Scene);
	});
}

void FRenderThread::UpdateRenderThreadScene(FScene* Scene)
{
	AddTask([this, Scene]{
		Renderer.UpdateResource(Scene);
	});
}

void FRenderThread::DrawThreadThreadScene(FScene* Scene)
{
	AddTask([this, Scene] {
		InitRenderBegin(FrameIndex, ClearColor);
		Renderer.Render(Scene);
		PresentToScreen(FrameIndex);
		FrameIndex = (FrameIndex + 1) % FrameCount;
		--FrameTaskNum;
		RenderCV.notify_all();
	});
}