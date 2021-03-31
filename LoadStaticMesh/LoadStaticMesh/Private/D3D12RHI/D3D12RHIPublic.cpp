
#include "stdafx.h"
#include "D3D12RHIPrivate.h"
#include "D3D12Helper.h"

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

DynamicRHI* D3D12DynamicRHIModule::CreateRHI()
{
	GD3D12RHI = new D3D12DynamicRHI(ChosenAdapter);
	return GD3D12RHI;
}

void D3D12DynamicRHIModule::FindAdapter()
{
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

void D3D12DynamicRHI::PostInit()
{

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