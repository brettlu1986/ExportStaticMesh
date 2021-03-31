
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Fence.h"
#include "D3D12Helper.h"

D3D12Adapter::D3D12Adapter(D3D12AdapterDesc& DescIn)
:OwningRHI(nullptr)
,Desc(DescIn)
,Device(nullptr)
,Fence(nullptr)
{

}

void D3D12Adapter::ShutDown()
{
	if(Device)
	{
		Device->ShutDown();
	}

	if(Fence)
	{
		Fence->ShutDown();
	}
	SwapChain.Reset();
	D3DDevice.Reset();
}

void D3D12Adapter::Initialize(D3D12DynamicRHI* RHI)
{
	OwningRHI = RHI;
	InitializeDevices();
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

	if(!D3DDevice)
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
			IID_PPV_ARGS(&D3DDevice));

		if (FAILED(HardwareResult))
		{
			ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&DxgiAdapter)));
			ThrowIfFailed(D3D12CreateDevice(
				GetAdapter(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&D3DDevice)));
		}
		NAME_D3D12_OBJECT(D3DDevice);
		
		Device = new D3D12Device(this);
		Device->Initialize();

		Fence = new D3D12Fence("GPUFence", this);
		Fence->Initialize();
	}
}

void D3D12Adapter::SetViewPort(const RHIViewPort& InViewPort)
{
	ViewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, InViewPort.Width, InViewPort.Height);
}

void D3D12Adapter::SetScissorRect(const RHIScissorRect& InRect)
{
	ScissorRect = CD3DX12_RECT(InRect.Left, InRect.Top, InRect.Right, InRect.Bottom);
}

void D3D12Adapter::CreateSwapChain(const RHISwapObjectInfo& SwapInfo)
{	
	SwapChain.Reset();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC Sd;
	Sd.BufferDesc.Width = SwapInfo.Width;
	Sd.BufferDesc.Height = SwapInfo.Height;
	Sd.BufferDesc.RefreshRate.Numerator = 60;
	Sd.BufferDesc.RefreshRate.Denominator = 1;
	Sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Sd.SampleDesc.Count = 1;
	Sd.SampleDesc.Quality = 0;
	Sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Sd.BufferCount = SwapInfo.ObjectCount;
	Sd.OutputWindow = (HWND)SwapInfo.WindowHandle;
	Sd.Windowed = true;
	Sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(DxgiFactory->CreateSwapChain(
		Device->GetD3DCommandQueue(),
		&Sd,
		&SwapChain));
}