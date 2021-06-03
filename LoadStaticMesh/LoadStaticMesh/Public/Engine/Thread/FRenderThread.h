#pragma once

#include "FTaskThread.h"
#include "FSceneRenderer.h"

class FScene;
class FRenderThread : public FTaskThread
{
public:
	FRenderThread();

	virtual void Run() override;
	void OnThreadInit();

	static bool IsInited();
	static void CreateRenderThread();
	static void DestroyRenderThread();
	static FRenderThread* Get();

	FScene* GetRenderScene()
	{
		return RenderScene.get();
	}

	/*void NotifyRenderThreadExcute();
	void NotifyGameExcute();
	bool ShouldWaitRender();
	bool ShouldWaitGame();*/
	void Clear();
	
	void WaitForRenderThread();

	condition_variable RenderCV;
	atomic_int SyncCount;

private:
	static FRenderThread* RenderThread;
	static const INT CPU_MAX_AHEAD = 3;
	//UINT SyncCount = 0;

	static bool Inited;
	FSceneRenderer Renderer;
	unique_ptr<FScene> RenderScene;
};

#define RENDER_THREAD_TASK(...) FRenderThread::Get()->AddTask(__VA_ARGS__)



