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

	condition_variable RenderCV;
	atomic_int FrameTaskNum;

	//void InitRenderThreadScene(FScene* Scene);
	//void UpdateRenderSceneResource(FScene* Scene);

	//void OnRenderScene(FScene* Scene);
	//void DestroyRenderScene(FScene* Scene);

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

private:
	static FRenderThread* RenderThread;
	//static const UINT CPU_MAX_AHEAD = 3;

	static bool Inited;

	FSceneRenderer Renderer;
	//FScene* RenderScene;
	
	//UINT SyncCount = 0;

	unique_ptr<FScene> RenderScene;
};

//TODO:we should not put wait here, this will delay the frame execute, influence the frame rate
#define RENDER_THREAD_TASK(...)\
FRenderThread::Get()->WaitForRenderThread(); \
FRenderThread::Get()->AddTask(__VA_ARGS__) \



