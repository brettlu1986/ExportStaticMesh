

#include "stdafx.h"
#include "LogicLoadModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <iostream>
#include "ApplicationMain.h"
#include "d3dx12.h"
#include "FDDSTextureLoader.h"

#include "FRHI.h"

using namespace Microsoft::WRL;
using namespace DirectX;

static const wstring TexFileName = L"Resource/T_Chair_M.dds";
static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

LogicLoadModel* LogicLoadModel::ThisLogic = nullptr;

LogicLoadModel::LogicLoadModel()
	: FrameIndex(0)
	, bDestroy(false)
	, ObjectConstant({})
	,ThreadParam(ThreadParameter())
{
	MtProj = MathHelper::Identity4x4();
	RenderBegin = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	RenderEnd = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	LastMousePoint = {0 , 0};
	ThisLogic = this;
}

LogicLoadModel::~LogicLoadModel()
{

}

void LogicLoadModel::OnInit()
{
	InitCamera();
	InitModel();
	CreateRenderThread();
}

void LogicLoadModel::CreateRenderThread()
{
	struct ThreadWrapper
	{
		static unsigned int WINAPI Thunk(void* LParameter)
		{
			LogicLoadModel::Get()->Render(LParameter);
			return 0;
		}
	};
	
	ThreadParam.ThisThread = reinterpret_cast<HANDLE>(_beginthreadex(
		nullptr, 
			0, 
		ThreadWrapper::Thunk,
		(void*)&ThreadParam, 
		CREATE_SUSPENDED, 
		(UINT*)&ThreadParam.ThreadId));
	
	ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	ResumeThread(ThreadParam.ThisThread);
}


void LogicLoadModel::Update()
{
	DWORD Ret = WaitForSingleObject(RenderEnd, INFINITE);
	if (Ret - WAIT_OBJECT_0 == 0)
	{
		//update matrix
		XMMATRIX WorldViewProj = ModelGeo.DataComponent->GetModelMatrix() * MyCamera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
		//Update the constant buffer with the latest WorldViewProj matrix.
		XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
		GDynamicRHI->RHIUpdateConstantBuffer(&ObjectConstant, sizeof(ObjectConstant));
		SetEvent(RenderBegin);
	}
	
}

void LogicLoadModel::RenderThreadInit()
{
	//create adapter , create device, create command queue, create command manager, create command allocator/list
	RHIInit();
	FRHIViewPort ViewPort = FRHIViewPort(static_cast<float>(WndWidth), static_cast<float>(WndHeight));
	GDynamicRHI->RHICreateViewPort(ViewPort);

	FRHISwapObjectInfo SwapObj = FRHISwapObjectInfo(WndWidth, WndHeight, FrameCount, MainApplication->GetHwnd());
	GDynamicRHI->RHICreateSwapObject(SwapObj);

	// load shader object
	UINT8* pVs = nullptr;
	UINT VsLen = 0;
	UINT8* pPs = nullptr;
	UINT PsLen = 0;

	GDynamicRHI->RHIReadShaderDataFromFile(GetAssetFullPath(L"shader_vs.cso"), &pVs, &VsLen);
	GDynamicRHI->RHIReadShaderDataFromFile(GetAssetFullPath(L"shader_ps.cso"), &pPs, &PsLen);

	FRHIInputElement RHIInputElementDescs[] =
	{
		{ "POSITION", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	FRHIPiplineStateInitializer RHIPsoInitializer = {
		RHIInputElementDescs,
		_countof(RHIInputElementDescs),
		pVs,
		VsLen,
		pPs,
		PsLen,
		1
	};
	GDynamicRHI->RHICreatePiplineStateObject(RHIPsoInitializer);
	GDynamicRHI->RHICreateRenderTarget(FrameCount);

	//
	UINT BufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	GDynamicRHI->RHICreateConstantBuffer(BufferSize, &ObjectConstant, sizeof(ObjectConstant));
	//
	GDynamicRHI->RHICreateDepthStencilBuffer(WndWidth, WndHeight);

	//create model vertex buffer view
	std::vector<Vertex_PositionTex0> TriangleVertices;
	ModelGeo.DataComponent->GetPositionTex0Input(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(Vertex_PositionTex0));
	const UINT StrideInByte = sizeof(Vertex_PositionTex0);
	ModelGeo.RenderComponent->RHIVertexBufferView = GDynamicRHI->RHICreateVertexBufferView(TriangleVertices.data(), StrideInByte, VertexBufferSize);

	//create model index buffer view
	ModelGeo.RenderComponent->RHIIndexBufferView = GDynamicRHI->RHICreateIndexBufferView(ModelGeo.DataComponent->IndicesData, ModelGeo.DataComponent->IndiceSize,
		ModelGeo.DataComponent->IndicesCount, ModelGeo.DataComponent->IsUseHalfInt32());

	//create model shader resource view
	ModelGeo.RenderComponent->RHIShaderResourceView = GDynamicRHI->RHICreateShaderResourceView(TexFileName);

	//first flush
	FRHICommandList& CommandList = GDynamicRHI->RHIGetCommandList(0);
	GDynamicRHI->RHIExcuteCommandList(CommandList);
	GDynamicRHI->RHISignalCurrentFence();
}

void LogicLoadModel::RenderThreadRun()
{
	if (!GDynamicRHI->RHIIsFenceComplete())
		return ;

	FRHICommandList& CommandList = GDynamicRHI->RHIGetCommandList(0);
	GDynamicRHI->RHIResetCommandList(CommandList);
	GDynamicRHI->RHISetCurrentViewPortAndScissorRect(CommandList);

	GDynamicRHI->RHITransitionToState(CommandList, FrameIndex, ETransitionState::STATE_RENDER);

	GDynamicRHI->RHIClearRenderTargetAndDepthStencilView(CommandList, FrameIndex, ClearColor);
	GDynamicRHI->RHISetGraphicRootDescripterTable(CommandList);

	ModelGeo.RenderComponent->Render(CommandList);

	GDynamicRHI->RHITransitionToState(CommandList, FrameIndex, ETransitionState::STATE_PRESENT);
	GDynamicRHI->RHIExcuteCommandList(CommandList);
	GDynamicRHI->RHISwapObjectPresent();

	GDynamicRHI->RHISignalCurrentFence();
	FrameIndex = (FrameIndex + 1) % FrameCount;
}

bool LogicLoadModel::Render(void* Param)
{
	try
	{
		RenderThreadInit();
		SetEvent(RenderEnd);
		DWORD Ret = 0;
		while (!bDestroy)
		{
			Ret = WaitForSingleObject(RenderBegin, INFINITE);
			if (Ret - WAIT_OBJECT_0 == 0)
			{
				RenderThreadRun();
			}

			SetEvent(RenderEnd);
		}
	}
	catch (HrException& e)
	{
		if (e.Error() == DXGI_ERROR_DEVICE_REMOVED || e.Error() == DXGI_ERROR_DEVICE_RESET)
		{
			RHIExit();
		}
		else
		{
			throw;
		}
	}
	return true;
}

void LogicLoadModel::Destroy()
{	
	bDestroy = true;
	//wait gpu to excute finish
	GDynamicRHI->RHISignalCurrentFence();

	RHIExit();
	ModelGeo.Destroy();
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

void LogicLoadModel::InitCamera()
{
	MyCamera.Init();
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	MyCamera.OnResize(static_cast<float>(WndWidth), static_cast<float>(WndHeight));
	XMMATRIX P = MyCamera.GetProjectionMatrix();
	XMStoreFloat4x4(&MtProj, P);

}

void LogicLoadModel::InitModel()
{
	ModelGeo.Init();
	ModelGeo.DataComponent->SetModelLocation(MyCamera.GetViewTargetLocation());

}

void LogicLoadModel::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePoint.x = x; 
	LastMousePoint.y = y;
	SetCapture(MainApplication->GetHwnd());
}

void LogicLoadModel::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void LogicLoadModel::OnMouseMove(WPARAM btnState, int x, int y)
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
