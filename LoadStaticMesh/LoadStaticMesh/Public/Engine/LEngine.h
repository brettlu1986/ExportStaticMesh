#pragma once

#include "stdafx.h"
#include "LDefine.h"
#include "LEvent.h"

typedef struct LEngineDesc
{
	UINT  Width;
	UINT  Height;
	std::string Name;
}LEngineDesc;

class LDevice;
class FScene;
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

	void InitRenderThreadScene(FScene* Scene);
	void WaitForRenderThread();
	void UpdateRenderThreadScene(FScene* Scene);
	void DrawThreadThreadScene(FScene* Scene);

private: 
	static LEngine* Engine;
	LDevice* CurrentDevice = nullptr;
	E_DEVICE_PLATFORM CurrentPlatform;
	EventDispatcher EventDisp;
};