#pragma once

#include "LogicStaticModel.h"
#include "LCamera.h"
//#include "FTempMesh.h"
#include "MathHelper.h"
#include "ApplicationMain.h"

#include "FScene.h"
#include "FMesh.h"
#include "FSceneRenderer.h"
#include "LDefine.h"

using namespace DirectX;

class LogicStaticModel 
{
public:
	LogicStaticModel();
	~LogicStaticModel();

	static LogicStaticModel* Get() { return ThisLogic; }

	void Initialize(ApplicationMain* Application, UINT Width, UINT Height);

	void OnInit();
	void Update();
	void Destroy();
	bool Render();

	static void ProcessInput(FInputResult Input);
	void ProcessMouseInput(FInputResult& Input);

private:

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	static LogicStaticModel* ThisLogic;

	void InitCamera();
	void InitModelScene();
	void RenderInit();

private:
	static const UINT FrameCount = 3;
	static const UINT ThreadCount = 1;

	bool bDestroy;
	ObjectConstants ObjectConstant;
	UINT FrameIndex;

	LCamera MyCamera;

	XMFLOAT4X4 MtProj;
	POINT LastMousePoint;

	HANDLE RenderBegin;
	HANDLE RenderEndHandle;
	float AspectRatio;

	UINT WndWidth;
	UINT WndHeight;
	ApplicationMain* MainApplication;

	FScene Scene;
	FMesh Mesh;
	FSceneRenderer Renderer;
	
};