

#include "stdafx.h"
#include "LogicStaticModel.h"

#include <DirectXColors.h>
#include <iostream>
#include "ApplicationMain.h"
#include "d3dx12.h"
#include "FDDSTextureLoader.h"
#include "FD3D12Helper.h"
#include "FRHI.h"

#include "LDeviceWindows.h"
#include "LEngine.h"
#include "SampleAssets.h"
#include "LAssetDataLoader.h"


using namespace Microsoft::WRL;
using namespace DirectX;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
LogicStaticModel* LogicStaticModel::ThisLogic = nullptr;

LogicStaticModel::LogicStaticModel()
	: FrameIndex(0)
	, bDestroy(false)
	, bRenderDestroy(false)
{
	MtProj = MathHelper::Identity4x4();
	LastMousePoint = {0 , 0};
	ThisLogic = this;
	MtBuffer = new RingBuffer<int8_t*>(FrameCount);
	MtBuffer->Clear();
}

LogicStaticModel::~LogicStaticModel()
{

}

void LogicStaticModel::Initialize() 
{
	EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
	EventDisp.RegisterEvent(new LEvent<FInputResult>(E_EVENT_KEY::EVENT_INPUT, &LogicStaticModel::ProcessInput));
	OnInit();
}

void LogicStaticModel::OnInit()
{
	InitCamera();
	InitModelScene();
	CreateRenderThread();
}

void LogicStaticModel::InitCamera()
{
	LAssetDataLoader::LoadCameraDataFromFile(SampleAssets::CameraBin, MyCamera);
	MyCamera.Init();
	XMMATRIX P = MyCamera.GetProjectionMatrix();
	XMStoreFloat4x4(&MtProj, P);
}

void LogicStaticModel::InitModelScene()
{
	for (UINT i = 0; i < SampleAssets::SamepleCount; i++)
	{
		FMesh* Mesh = new FMesh(SampleAssets::SampleResources[i], i, SampleAssets::SampleResourceTexture[i],
			SampleAssets::SampleResourceTexture[i] != "" ? SampleAssets::PsoUseTexture : SampleAssets::PsoNoTexture);
		Scene.AddMeshToScene(Mesh);
	}

	ConstantBufferSingleSize = (sizeof(ObjectConstants) + 255) & ~255;
	ConstantBufferTotalSize = ConstantBufferSingleSize * SampleAssets::SamepleCount;
	
}

// main thread
void LogicStaticModel::Update()
{
	//update matrix
	if(!bDestroy)
	{
		XMMATRIX WorldViewProj;
		XMFLOAT4X4 WVP;
		XMMATRIX Model;
		
		int8_t* ConstantBuffers = new int8_t[ConstantBufferTotalSize];
		memset(ConstantBuffers, 0, ConstantBufferTotalSize);
		for (UINT i = 0; i < SampleAssets::SamepleCount; i++)
		{
			Model = Scene.GetDrawMeshes()[i]->GetModelMatrix();
			WorldViewProj = Model * MyCamera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
			//Update the constant buffer with the latest WorldViewProj matrix.
			XMStoreFloat4x4(&WVP, XMMatrixTranspose(WorldViewProj));
			memcpy( ConstantBuffers + i * ConstantBufferSingleSize, &WVP, sizeof(WVP));
		}
		MtBuffer->Enqueue(ConstantBuffers);
	}
}

// render thread
void LogicStaticModel::CreateRenderThread()
{
	auto Run = [this]() {
		RenderInit();
		while (!(MtBuffer->IsEmpty() && bDestroy))
		{
			int8_t* ConstantObj = nullptr;
			if (MtBuffer->Dequeue(&ConstantObj))
			{
				UpdateConstantBuffer(ConstantObj, ConstantBufferTotalSize);
				delete ConstantObj;
			}
			Render();
		}

		Scene.Destroy();
		RHIExit();
		bRenderDestroy = true;
	};

	auto Exit = [this]() {
		
	};

	RThread.Start(Run, Exit);
	RThread.Detach();
}


void LogicStaticModel::RenderInit()
{	
	RHIInit();
	Renderer.RenderInit(&Scene);
}

bool LogicStaticModel::Render()
{
	InitRenderBegin(FrameIndex, ClearColor);
	Renderer.Render(&Scene);
	PresentToScreen(FrameIndex);
	FrameIndex = (FrameIndex + 1) % FrameCount;
	return true;
}

void LogicStaticModel::ProcessInput(FInputResult Input)
{
	if(LInput::IsMouseInput(Input))
	{
		LogicStaticModel::Get()->ProcessMouseInput(Input);	
	}
}

void LogicStaticModel::ProcessMouseInput(FInputResult& Input)
{
	if (Input.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_DOWN)
	{
		LastMousePoint.x = Input.X;
		LastMousePoint.y = Input.Y;
	}
	else if(Input.TouchType == E_TOUCH_TYPE::MOUSE_LEFT_MOVE)
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(Input.X - LastMousePoint.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(Input.Y - LastMousePoint.y));
		MyCamera.ChangeViewMatrixByMouseEvent(dx, dy);
		LastMousePoint.x = Input.X;
		LastMousePoint.y = Input.Y;
	}
}

// destroy
void LogicStaticModel::Destroy()
{	
	bDestroy = true;
	while(!bRenderDestroy)
	{
		continue;
	}

	if(MtBuffer)
	{
		delete MtBuffer;
		MtBuffer = nullptr;
	}
	ThisLogic = nullptr;
}



