#pragma once

#include "stdafx.h"
#include "LDefine.h"
#include "LEvent.h"
#include "FRenderThread.h"

typedef struct LEngineDesc
{
	UINT  Width;
	UINT  Height;
	string Name;
}LEngineDesc;

class LDevice;
class LAssetManager;
class LEngine
{
public:
	static LEngine* GetEngine();

	static void InitEngine(LEngineDesc Desc);

	void Init(LEngineDesc Desc);
	bool Run();
	void Destroy();

	E_DEVICE_PLATFORM GetPlatform() const
	{
		return CurrentPlatform;
	}

	LDevice* GetPlatformDevice()
	{
		return CurrentDevice;
	}

	EventDispatcher& GetEventDispacher()
	{
		return EventDisp;
	}

	bool IsGameThread()
	{
		return this_thread::get_id() == GameThreadId;
	}

	bool IsRenderThread()
	{
		return this_thread::get_id() == FRenderThread::Get()->GetThreadId();
	}

private: 
	static LEngine* Engine;

	LAssetManager* AssetManager = nullptr;
	LDevice* CurrentDevice = nullptr;
	E_DEVICE_PLATFORM CurrentPlatform;
	EventDispatcher EventDisp;

	thread::id GameThreadId;
};