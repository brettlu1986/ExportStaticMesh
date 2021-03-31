
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

	RtvHeap.Reset();
	DsvHeap.Reset();
	CbvSrvHeap.Reset();
	SamplerHeap.Reset();
	SwapChain.Reset();
	RootSignature.Reset();
	D3DDevice.Reset();
}

void D3D12Adapter::Initialize(D3D12DynamicRHI* RHI)
{
	OwningRHI = RHI;
	InitializeDevices();
	CreateDescriptorHeaps();
	CreateSignature();
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

void D3D12Adapter::CreateDescriptorHeaps()
{
	//dsv
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&DsvHeap));
	NAME_D3D12_OBJECT(DsvHeap);
	//constant buffer and shader resource view buffer
	CreateDescripterHeap(2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&CbvSrvHeap));
	NAME_D3D12_OBJECT(CbvSrvHeap);
	//sample
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&SamplerHeap));
	NAME_D3D12_OBJECT(SamplerHeap);
}

void D3D12Adapter::CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flag, UINT NodeMask, REFIID riid,
	_COM_Outptr_  void** ppvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
	Desc.NumDescriptors = NumDescripters;
	Desc.Type = Type;
	Desc.Flags = Flag;
	Desc.NodeMask = NodeMask;
	ThrowIfFailed(GetD3DDevice()->CreateDescriptorHeap(&Desc, riid, ppvHeap));
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

    // rtv create here because we can use swap count to create rtv count
	CreateDescripterHeap(SwapInfo.ObjectCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&RtvHeap));
	NAME_D3D12_OBJECT(RtvHeap);
}

void D3D12Adapter::CreateSignature()
{
	CD3DX12_ROOT_PARAMETER RootParameters[3];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[3];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0);
	Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	Ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[1].InitAsDescriptorTable(1, &Ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameters[2].InitAsDescriptorTable(1, &Ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(RootParameters), RootParameters, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> Signature;
	ComPtr<ID3DBlob> Error;
	
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
	ThrowIfFailed(GetD3DDevice()->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
	NAME_D3D12_OBJECT(RootSignature);
}