
#include "D3D12Adapter.h"
#include "GraphicHelper.h"

D3D12Adapter::D3D12Adapter(D3D12AdapterDesc& DescIn)
:OwningRHI(nullptr)
,Desc(DescIn)
{

}

void D3D12Adapter::Initialize(D3D12DynamicRHI* RHI)
{
	OwningRHI = RHI;
}

void D3D12Adapter::InitializeDevices()
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

	if(!RootDevice)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

		ComPtr<IDXGIFactory6> Factory6;
		if (SUCCEEDED(DxgiFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
		{
			Factory6->EnumAdapterByGpuPreference(
				Desc.AdapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&DxgiAdapter));
		}

		HRESULT HardwareResult = D3D12CreateDevice(
			GetAdapter(),             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&RootDevice));

		if (FAILED(HardwareResult))
		{
			ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&DxgiAdapter)));
			ThrowIfFailed(D3D12CreateDevice(
				GetAdapter(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&RootDevice)));
		}
		NAME_D3D12_OBJECT(RootDevice);
		
	}
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void D3D12Adapter::GetHardwareAdapter(
	IDXGIFactory1* pFactory,
	IDXGIAdapter1** ppAdapter,
	bool RequestHighPerformanceAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> Adapter;

	ComPtr<IDXGIFactory6> Factory6;
	if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
	{
		for (
			UINT AdapterIndex = 0;
			DXGI_ERROR_NOT_FOUND != Factory6->EnumAdapterByGpuPreference(
				AdapterIndex,
				RequestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&Adapter));
			++AdapterIndex)
		{
			DXGI_ADAPTER_DESC1 Desc;
			Adapter->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}
	else
	{
		for (UINT AdapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(AdapterIndex, &Adapter); ++AdapterIndex)
		{
			DXGI_ADAPTER_DESC1 Desc;
			Adapter->GetDesc1(&Desc);

			if (Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				// If you want a software adapter, pass in "/warp" on the command line.
				continue;
			}

			// Check to see whether the adapter supports Direct3D 12, but don't create the
			// actual device yet.
			if (SUCCEEDED(D3D12CreateDevice(Adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = Adapter.Detach();
}