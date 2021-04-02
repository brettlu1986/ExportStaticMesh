#pragma once

#include "Logic.h"
#include "LCamera.h"
#include "FMesh.h"
#include "MathHelper.h"

using namespace DirectX;

class LogicLoadModel : public Logic
{
public:
	LogicLoadModel();
	~LogicLoadModel();

	static LogicLoadModel* Get() { return ThisLogic; }

	virtual void OnInit();
	virtual bool Render(void* Param);
	virtual void Update();
	virtual void Destroy();

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

private:

	struct ObjectConstants
	{
		XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
	};

	struct ThreadParameter
	{
		UINT  ThreadId;
		HANDLE ThisThread;
	};
	ThreadParameter ThreadParam;
	static LogicLoadModel* ThisLogic;

	void InitCamera();
	void InitModel();
	void CreateRenderThread();
	void RenderThreadInit();
	void RenderThreadRun();

private:
	static const UINT FrameCount = 3;
	static const UINT ThreadCount = 1;

	bool bDestroy;
	ObjectConstants ObjectConstant;
	UINT FrameIndex;

	FMesh ModelGeo;
	LCamera MyCamera;
	
	XMFLOAT4X4 MtProj; 
	POINT LastMousePoint;

	HANDLE RenderBegin;
	HANDLE RenderEnd;
	
};