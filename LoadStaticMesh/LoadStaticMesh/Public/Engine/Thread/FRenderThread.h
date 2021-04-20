#pragma once

#include "FTaskThread.h"
#include "FSceneRenderer.h"

class FScene;
class FRenderThread : public FTaskThread
{
public:
	FRenderThread();

	virtual void Run() override;

	static void CreateRenderThread();
	static void DestroyRenderThread();
	static FRenderThread* Get();

	condition_variable RenderCV;
	atomic_int FrameTaskNum;

	void InitRenderThreadScene(FScene* Scene);

	/*void NotifyRenderThreadExcute();
	void NotifyGameExcute();
	bool ShouldWaitRender();
	bool ShouldWaitGame();*/
	void Clear();

	FScene* GetRenderScene()
	{
		return RenderScene;
	}
	
	void WaitForRenderThread();

private:
	static FRenderThread* RenderThread;
	static const UINT CPU_MAX_AHEAD = 3;
	FSceneRenderer Renderer;
	FScene* RenderScene;
	
	//UINT SyncCount = 0;
};

#define RENDER_THREAD(...) FRenderThread::Get()->AddTask(__VA_ARGS__)