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
	void WaitForRenderThread();

	condition_variable RenderCV;
	atomic_int FrameTaskNum;

	void InitRenderThreadScene(FScene* Scene);
	void UpdateRenderThreadScene(FScene* Scene);
	void DrawThreadThreadScene(FScene* Scene);
private: 
	static FRenderThread* RenderThread;
	FSceneRenderer Renderer;
	UINT FrameIndex;
};