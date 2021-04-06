

#include "stdafx.h"
#include "LogicStaticModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <iostream>
#include "ApplicationMain.h"
#include "d3dx12.h"
#include "FDDSTextureLoader.h"
#include "FD3D12Helper.h"
#include "FRHI.h"

#include "LDeviceWindows.h"
#include "LEngine.h"

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
	OnInit();
}


void LogicStaticModel::OnInit()
{
	InitCamera();
	RenderInit();
}

//void LogicStaticModel::UpdateInput()
//{
//	LDeviceWindows* Device = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
//	LInput* Input = Device->GetInput();
//	if(Input->IsMouseInput())
//	{
//	}
//}

void LogicStaticModel::Update()
{
	//UpdateInput();
	//update matrix
	XMMATRIX WorldViewProj = Mesh.GetModelMatrix() * MyCamera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
	//Update the constant buffer with the latest WorldViewProj matrix.
	XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	GDynamicRHI->RHIUpdateConstantBuffer(&ObjectConstant, sizeof(ObjectConstant));
}

void LogicStaticModel::RenderInit()
{
	InitModelScene();
	RHIInit();
	RHIInitWindow(WndWidth, WndHeight, MainApplication->GetHwnd());

	Renderer.RenderInit(&Scene);

	UINT BufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	GDynamicRHI->RHICreateConstantBuffer(BufferSize, &ObjectConstant, sizeof(ObjectConstant));
	
	//first flush
	FirstPresent();
}


bool LogicStaticModel::Render()
{
	InitRenderBegin(FrameIndex, ClearColor);
	Renderer.Render(&Scene);
	RenderEnd(FrameIndex);
	FrameIndex = (FrameIndex + 1) % FrameCount;
	return true;
}

void LogicStaticModel::Destroy()
{	
	bDestroy = true;
	//wait gpu to excute finish
	RHIExit();
	Scene.Destroy();
	//ModelGeo.Destroy();
	ThisLogic = nullptr;
#if defined(_DEBUG)
	{
		ComPtr<IDXGIDebug1> DxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
		{
			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
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

//mouse event
void LogicStaticModel::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePoint.x = x; 
	LastMousePoint.y = y;
	SetCapture(MainApplication->GetHwnd());
}

void LogicStaticModel::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void LogicStaticModel::OnMouseMove(WPARAM btnState, int x, int y)
{
	if((btnState & MK_LBUTTON) != 0 ) 
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePoint.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePoint.y));
		MyCamera.ChangeViewMatrixByMouseEvent(dx, dy);
	}
	LastMousePoint.x = x; 
	LastMousePoint.y = y;
}
