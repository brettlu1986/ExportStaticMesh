#pragma once

#include "LogicStaticModel.h"
#include "LCamera.h"
//#include "FTempMesh.h"
#include "MathHelper.h"
#include "ApplicationMain.h"

#include "FScene.h"
#include "FMesh.h"
#include "FSceneRenderer.h"

using namespace DirectX;

class LogicStaticModel 
{
public:
	LogicStaticModel();
	~LogicStaticModel();

	static LogicStaticModel* Get() { return ThisLogic; }

	void Initialize(ApplicationMain* Application, UINT Width, UINT Height);

	void OnInit();
	//bool Render(void* Param);
	void Update();
	void Destroy();
	bool Render();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	/*struct ThreadParameter
	{
		UINT  ThreadId;
		HANDLE ThisThread;
	};
	ThreadParameter ThreadParam;*/
	static LogicStaticModel* ThisLogic;

	void InitCamera();
	void InitModelScene();
	//void CreateRenderThread();
	void RenderThreadInit();
	void RenderThreadRun();

private:
	static const UINT FrameCount = 3;
	static const UINT ThreadCount = 1;

	bool bDestroy;
	ObjectConstants ObjectConstant;
	UINT FrameIndex;

	//FTempMesh ModelGeo;
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