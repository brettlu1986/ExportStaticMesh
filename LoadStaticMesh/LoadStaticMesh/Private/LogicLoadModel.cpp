

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
	CbBuffers = new RingBuffer<FBufferObject*>(FrameCount);
	CbBuffers->Clear();

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
		FMesh* Mesh = new FMesh(SampleAssets::SampleResources[i], SampleAssets::SampleResourceTexture[i],
			SampleAssets::SampleResourceTexture[i] != "" ? SampleAssets::PsoUseTexture : SampleAssets::PsoNoTexture);

		Mesh->InitMaterial(SampleAssets::SampeMats[i].Name, SampleAssets::SampeMats[i].DiffuseAlbedo, SampleAssets::SampeMats[i].FresnelR0,
			SampleAssets::SampeMats[i].Roughness);
		Scene.AddMeshToScene(Mesh);
	}

	MtBufferSingleSize = CalcConstantBufferByteSize(sizeof(FObjectConstants));
	MtBufferTotalSize = MtBufferSingleSize * Scene.GetMeshCount();
	
	MatBufferSingleSize = CalcConstantBufferByteSize(sizeof(FMaterialConstants));
	MatBufferTotalSize = MatBufferSingleSize * Scene.GetMeshCount();
}

// main thread
void LogicStaticModel::Update()
{
	//update matrix
	if(!bDestroy)
	{
		UpdateMtConstantBuffer();
	//	UpdateMatConstantBuffer();
	}
}

void LogicStaticModel::UpdateMtConstantBuffer()
{
	XMMATRIX WorldViewProj;
	XMFLOAT4X4 WVP;
	XMMATRIX Model;

	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX;

	BufferObj->BufferData = new int8_t[MtBufferTotalSize];
	memset(BufferObj->BufferData, 0, MtBufferTotalSize);
	for (UINT i = 0; i < Scene.GetMeshCount(); i++)
	{
		Model = Scene.GetDrawMeshes()[i]->GetModelMatrix();
		WorldViewProj = Model * MyCamera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
		//Update the constant buffer with the latest WorldViewProj matrix.
		XMStoreFloat4x4(&WVP, XMMatrixTranspose(WorldViewProj));
		memcpy(BufferObj->BufferData + i * MtBufferSingleSize, &WVP, sizeof(WVP));
	}
	CbBuffers->Enqueue(BufferObj);
}

void LogicStaticModel::UpdateMatConstantBuffer()
{
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL;

	BufferObj->BufferData = new int8_t[MatBufferTotalSize];
	memset(BufferObj->BufferData, 0, MatBufferTotalSize);

	XMFLOAT4X4 Mat;
	for (UINT i = 0; i < Scene.GetMeshCount(); i++)
	{
		FMaterial* Material = Scene.GetDrawMeshes()[i]->GetMaterial();
		Mat = Material->GetMaterialTransform();
		XMMATRIX MatTransform = XMLoadFloat4x4(&Mat);
		FMaterialConstants MatConstants;
		MatConstants.DiffuseAlbedo = Material->GetDiffuseAlbedo();
		MatConstants.FresnelR0 = Material->GetFresnelR0();
		MatConstants.Roughness = Material->GetRoughness();
		XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(MatTransform));

		memcpy(BufferObj->BufferData + i * MatBufferSingleSize, &MatConstants, sizeof(MatConstants));
	}
	CbBuffers->Enqueue(BufferObj);
}

// render thread
void LogicStaticModel::CreateRenderThread()
{
	auto Run = [this]() {
		RenderInit();
		while (!(CbBuffers->IsEmpty() && bDestroy))
		{
			FBufferObject* ConstantObj = nullptr;
			if (CbBuffers->Dequeue(&ConstantObj))
			{
				UpdateConstantBuffer(ConstantObj, MtBufferTotalSize);
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

	if(CbBuffers)
	{
		delete CbBuffers;
		CbBuffers = nullptr;
	}
	ThisLogic = nullptr;
}



