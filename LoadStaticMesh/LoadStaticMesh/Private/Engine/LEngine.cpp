#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRHI.h"
#include "FRenderThread.h"
#include "FScene.h"

LEngine* LEngine::Engine = nullptr;

LEngine* LEngine::GetEngine()
{
	return Engine;
}

void LEngine::InitEngine(LEngineDesc Desc)
{
	if(!Engine)
	{
		Engine = new LEngine();
		Engine->Init(Desc);
	}
}

bool LEngine::Run()
{
	return CurrentDevice->Run();
}

void LEngine::Render()
{

}

void LEngine::Init(LEngineDesc Desc)
{

#ifdef PLATFORM_WINDOWS 
	CurrentPlatform = E_DEVICE_PLATFORM::DEVICE_WINDOWS;
	CurrentDevice = new LDeviceWindows(Desc.Width, Desc.Height, Desc.Name.c_str());
#else 
#endif
	//only create the module, not call any init rhi api
	RHICreate();

	FRenderThread::CreateRenderThread();
	FRenderThread::Get()->Start();
}

void LEngine::InitRenderThreadScene(FScene* Scene)
{
	FRenderThread::Get()->InitRenderThreadScene(Scene);
}

void LEngine::WaitForRenderThread()
{
	FRenderThread::Get()->WaitForRenderThread();
}

void LEngine::UpdateRenderThreadScene(FScene* Scene)
{
	FRenderThread::Get()->UpdateRenderThreadScene(Scene);
}

void LEngine::DrawThreadThreadScene(FScene* Scene)
{
	FRenderThread::Get()->DrawThreadThreadScene(Scene);
}


void LEngine::Destroy()
{
	FRenderThread::DestroyRenderThread();

	if(CurrentDevice)
	{
		delete CurrentDevice;
		CurrentDevice = nullptr;
	}

	Engine = nullptr;
}

