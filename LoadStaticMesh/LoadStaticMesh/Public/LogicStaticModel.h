#pragma once

#include "LogicStaticModel.h"
#include "LCamera.h"
#include "MathHelper.h"
#include "ApplicationMain.h"

#include "FScene.h"
#include "FMesh.h"
#include "FSceneRenderer.h"
#include "LDefine.h"
#include "LRingBuffer.h"
#include "RenderThread.h"
#include "FDefine.h"

using namespace DirectX;

class LogicStaticModel 
{
public:
	LogicStaticModel();
	~LogicStaticModel();

	static LogicStaticModel* Get() { return ThisLogic; }
	void Initialize();

	void OnInit();
	void Update();
	void Destroy();
	bool Render();

	static void ProcessInput(FInputResult Input);
	void ProcessMouseInput(FInputResult& Input);

private:
	static LogicStaticModel* ThisLogic;

	void InitCamera();
	void InitModelScene();
	void RenderInit();
	void CreateRenderThread();

private:
	void UpdateMtConstantBuffer();
	void UpdateMatConstantBuffer();

	static const UINT FrameCount = 3;

	bool bDestroy;
	bool bRenderDestroy;
	UINT FrameIndex;

	LCamera MyCamera;

	XMFLOAT4X4 MtProj;
	POINT LastMousePoint;


	FScene Scene;
	FSceneRenderer Renderer;

	RingBuffer<FBufferObject*>* CbBuffers;
	RenderThread RThread;

	UINT MtBufferSingleSize;
	UINT MtBufferTotalSize;
	
	UINT MatBufferSingleSize;
	UINT MatBufferTotalSize;
};