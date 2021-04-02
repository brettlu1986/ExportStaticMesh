
#include "stdafx.h"
#include "D3D12RHIPrivate.h"
#include "D3D12Helper.h"
#include "D3D12Device.h"
#include "D3D12ResourceManager.h"

using namespace Microsoft::WRL;
using namespace DirectX;

D3D12DynamicRHI* GD3D12RHI = nullptr;

bool D3D12DynamicRHIModule::IsSupported()
{
	if(ChosenAdapter == nullptr)
	{
		FindAdapter();
	}
	return ChosenAdapter != nullptr;
}

FDynamicRHI* D3D12DynamicRHIModule::CreateRHI()
{
	GD3D12RHI = new D3D12DynamicRHI(ChosenAdapter);
	return GD3D12RHI;
}

void D3D12DynamicRHIModule::FindAdapter()
{
#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> DebugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController))))
		{
			DebugController->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> DxgiFactory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

	ComPtr<IDXGIAdapter1> Adapter;
	ComPtr<IDXGIFactory6> Factory6;

	ID3D12Device* pDevice = nullptr;
	const D3D_FEATURE_LEVEL MinRequiredFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);
	DXGI_ADAPTER_DESC1 Desc;
	UINT Index;
	if (SUCCEEDED(DxgiFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (
			UINT AdapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != Factory6->EnumAdapterByGpuPreference(
				AdapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&Adapter));
			++AdapterIndex)
		{
			Adapter->GetDesc1(&Desc);
			Index = AdapterIndex;
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
			{
				break;
			}
		}
	}
	else
	{
		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != DxgiFactory->EnumAdapters1(AdapterIndex, &Adapter); ++AdapterIndex)
		{
			Adapter->GetDesc1(&Desc);
			Index = AdapterIndex;
			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}
			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&pDevice))))
			{
				break;
			}
		}
	}
	D3D12AdapterDesc CurrentAdapter(Desc, Index, MaxSupportedFeatureLevel, pDevice->GetNodeCount());
	ChosenAdapter = new D3D12Adapter(CurrentAdapter);
	pDevice->Release();
}

/////////////////////////// dynamic RHI implement
D3D12DynamicRHI::D3D12DynamicRHI(D3D12Adapter* ChosenAdapterIn)
{
	ChosenAdapter = ChosenAdapterIn;
}

void D3D12DynamicRHI::Init() 
{
	if(ChosenAdapter)
	{
		ChosenAdapter->Initialize(this);
	}
}

void D3D12DynamicRHI::ShutDown()
{
	if(ChosenAdapter)
	{
		ChosenAdapter->ShutDown();
		ChosenAdapter = nullptr;
	}
}


GenericFence* D3D12DynamicRHI::RHICreateFence(const std::string& Name) 
{
	return new D3D12Fence(Name, ChosenAdapter);
}


void D3D12DynamicRHI::RHICreateViewPort(RHIViewPort& ViewPort)
{
	ChosenAdapter->SetViewPort(ViewPort);
	RHIScissorRect Rect = RHIScissorRect(0, 0, static_cast<LONG>(ViewPort.Width), static_cast<LONG>(ViewPort.Height));
	ChosenAdapter->SetScissorRect(Rect);
}

void D3D12DynamicRHI::RHICreateSwapObject(RHISwapObjectInfo& SwapInfo) 
{
	ChosenAdapter->CreateSwapChain(SwapInfo);
}

void D3D12DynamicRHI::RHICreatePiplineStateObject(RHIPiplineStateInitializer& Initializer) 
{
	ChosenAdapter->CreatePso(Initializer);
}

void D3D12DynamicRHI::RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size)
{
	ThrowIfFailed(ReadDataFromFile(FileName.c_str(), Data, Size));
}

D3D12ResourceManager* D3D12DynamicRHI::GetResourceManager()
{
	D3D12Device* Device = ChosenAdapter->GetDevice();
	return Device->GetResourceManager();
}

void D3D12DynamicRHI::RHICreateRenderTarget(UINT TargetCount)
{
	GetResourceManager()->CreateRenderTarget(TargetCount);
}

void D3D12DynamicRHI::RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
{
	GetResourceManager()->CreateConstantBuffer(BufferSize, pDataFrom, DataSize);
}

void D3D12DynamicRHI::RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize)
{
	GetResourceManager()->UpdateConstantBufferData(pUpdateData, DataSize);
}

void D3D12DynamicRHI::RHICreateDepthStencilBuffer(UINT Width, UINT Height) 
{
	GetResourceManager()->CreateDepthStencilBuffer(Width, Height);
}

RHIView* D3D12DynamicRHI::RHICreateVertexBufferView(const void* InitData, UINT StrideInByte, UINT DataSize)
{
	return GetResourceManager()->CreateVertexBufferView(InitData, StrideInByte, DataSize);
}

RHIView* D3D12DynamicRHI::RHICreateIndexBufferView(const void* InitData, UINT DataSize, UINT IndicesCount, bool bUseHalfInt32)
{
	return GetResourceManager()->CreateIndexBufferView(InitData, DataSize, IndicesCount, bUseHalfInt32);
}

RHIView* D3D12DynamicRHI::RHICreateShaderResourceView(std::wstring TextureName)
{
	return GetResourceManager()->CreateShaderResourceView(TextureName);
}

RHICommandList& D3D12DynamicRHI::RHIGetCommandList(UINT Index)
{
	return ChosenAdapter->GetDevice()->GetCommandListManager()->GetCommandList(Index);
}

void D3D12DynamicRHI::RHIExcuteCommandList(RHICommandList& CommandList)
{
	CommandList.Close();
	CommandList.Excute();
}

void D3D12DynamicRHI::RHICloseCommandList(RHICommandList& CommandList)
{
	CommandList.Close();
}

void D3D12DynamicRHI::RHISignalCurrentFence()
{
	D3D12Fence* Fence = ChosenAdapter->GetFence();
	Fence->SignalCurrentFence();
}

bool D3D12DynamicRHI::RHIIsFenceComplete()
{
	D3D12Fence* Fence = ChosenAdapter->GetFence();
	return Fence->IsFenceComplete();
}

void D3D12DynamicRHI::RHIResetCommandList(RHICommandList& CommandList)
{
	CommandList.Reset();
}

void D3D12DynamicRHI::RHISetCurrentViewPortAndScissorRect(RHICommandList& CommandList)
{
	CommandList.SetViewPort();
	CommandList.SetScissorRect();
}

void D3D12DynamicRHI::RHITransitionToState(RHICommandList& CommandList, UINT TargetFrame, ETransitionState State)
{
	CommandList.TransitionToState(TargetFrame, State);
}

void D3D12DynamicRHI::RHIClearRenderTargetAndDepthStencilView(RHICommandList& CommandList, UINT TargetFrame, RHIColor ClearColor)
{
	CommandList.ClearRenderTargetAndDepthStencilView(TargetFrame, ClearColor);
}

 void D3D12DynamicRHI::RHISetGraphicRootDescripterTable(RHICommandList& CommandList)
 {
	 CommandList.SetGraphicRootDescripterTable();
 }

 void D3D12DynamicRHI::RHIDrawWithVertexAndIndexBufferView(RHICommandList& CommandList, RHIView* VertexBufferView, RHIView* IndexBufferView)
 {
	 CommandList.DrawWithVertexAndIndexBufferView(VertexBufferView, IndexBufferView);
 }

 void D3D12DynamicRHI::RHISwapObjectPresent()
 {
	 IDXGISwapChain* SwapChain = ChosenAdapter->GetSwapChain();
	 SwapChain->Present(1, 0);
 }