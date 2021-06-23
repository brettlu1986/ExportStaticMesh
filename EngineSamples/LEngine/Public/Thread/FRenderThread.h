#pragma once

#include "FTaskThread.h"
#include "FSceneRenderer.h"

class FScene;
class FRenderThread : public FTaskThread
{
public:
	FRenderThread();

	virtual void Run() override;
	virtual void Stop() override;
	void OnThreadInit();

	static bool IsInited();
	static void CreateRenderThread();
	static void DestroyRenderThread();
	LENGINE_API static FRenderThread* Get();

	FScene* GetRenderScene()
	{
		return RenderScene.get();
	}

	void Clear();
	
	LENGINE_API void WaitForRenderThread();
	LENGINE_API void WaitForGameThread();

	condition_variable RenderCV;
	

private:
	static FRenderThread* RenderThread;

	atomic_int SyncCount = 0;
	static const INT CPU_MAX_AHEAD = 2;

	static bool Inited;
	FSceneRenderer Renderer;
	unique_ptr<FScene> RenderScene;
};

#define RENDER_THREAD_TASK(...) FRenderThread::Get()->AddTask(__VA_ARGS__)



