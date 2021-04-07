

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
#include "LRingBuffer.h"


#include "LAssetDataLoader.h"

using namespace Microsoft::WRL;
using namespace DirectX;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

LogicStaticModel* LogicStaticModel::ThisLogic = nullptr;

LogicStaticModel::LogicStaticModel()
	: FrameIndex(0)
	, bDestroy(false)
	, ObjectConstant({})
	, WndWidth(0)
	, WndHeight(0)
	, AspectRatio(0.f)
	, MainApplication(nullptr)
{
	MtProj = MathHelper::Identity4x4();
	RenderBegin = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	RenderEndHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	LastMousePoint = {0 , 0};
	ThisLogic = this;
	
}

LogicStaticModel::~LogicStaticModel()
{

}

void LogicStaticModel::Initialize(ApplicationMain* Application, UINT Width, UINT Height)
{
	MainApplication = Application;
	WndWidth = Width;
	WndHeight = Height;
	AspectRatio = static_cast<float>(Width) / static_cast<float>(Height);
	EventDispatcher& EventDisp = LEngine::GetEngine()->GetEventDispacher();
	EventDisp.RegisterEvent(new LEvent<FInputResult>(E_EVENT_KEY::EVENT_INPUT, &LogicStaticModel::ProcessInput));
	OnInit();
}


void LogicStaticModel::OnInit()
{
	InitCamera();
	
	RenderInit();
}

void LogicStaticModel::Update()
{
	//update matrix
	XMMATRIX WorldViewProj = Mesh.GetModelMatrix() * MyCamera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
	//Update the constant buffer with the latest WorldViewProj matrix.
	XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	UpdateConstantBuffer(&ObjectConstant, sizeof(ObjectConstant));
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

void LogicStaticModel::RenderInit()
{
	InitModelScene();
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

void LogicStaticModel::Destroy()
{	
	bDestroy = true;
	//wait gpu to excute finish
	Scene.Destroy();
	RHIExit();
	ThisLogic = nullptr;
}

void LogicStaticModel::InitCamera()
{
	LAssetDataLoader::LoadCameraDataFromFile("camera.bin", MyCamera);
	MyCamera.Init();

	// The window resized, so update the aspect ratio and recompute the projection matrix.
	MyCamera.OnResize(static_cast<float>(WndWidth), static_cast<float>(WndHeight));
	XMMATRIX P = MyCamera.GetProjectionMatrix();
	XMStoreFloat4x4(&MtProj, P);
}

void LogicStaticModel::InitModelScene()
{
	Mesh = FMesh("mesh.bin", "Resource/T_Chair_M.dds");
	Mesh.SetModelLocation(MyCamera.GetViewTargetLocation());
	Scene.AddMeshToScene(&Mesh);
}

