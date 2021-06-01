
#include "stdafx.h"
#include "LEngine.h"
#include "LDeviceWindows.h"
#include "FRHI.h"

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

void LEngine::Init(LEngineDesc Desc)
{

#ifdef PLATFORM_WINDOWS 
	CurrentPlatform = E_DEVICE_PLATFORM::DEVICE_WINDOWS;
	CurrentDevice = new LDeviceWindows(Desc.Width, Desc.Height, Desc.Name.c_str());
#else 
#endif

	GameThreadId = this_thread::get_id();

	FRenderThread::CreateRenderThread();
	FRenderThread::Get()->Start();

	while(!FRenderThread::IsInited())
	{
		continue;
	}
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

