#pragma once

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
	LENGINE_API static LEngine* GetEngine();

	LENGINE_API static void InitEngine(LEngineDesc Desc);

	LENGINE_API void Init(LEngineDesc Desc);
	LENGINE_API bool Run();
	LENGINE_API void Destroy();

	LENGINE_API E_DEVICE_PLATFORM GetPlatform() const
	{
		return CurrentPlatform;
	}

	LENGINE_API LDevice* GetPlatformDevice()
	{
		return CurrentDevice;
	}

	LENGINE_API EventDispatcher& GetEventDispacher()
	{
		return EventDisp;
	}

	LENGINE_API bool IsGameThread()
	{
		return this_thread::get_id() == GameThreadId;
	}

	LENGINE_API bool IsRenderThread()
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