
#include "FD3D12Adapter.h"
#include "FD3D12Device.h"
#include "FD3D12Fence.h"
#include "FD3D12Helper.h"
#include "d3dx12.h"

FD3D12Adapter::FD3D12Adapter(D3D12AdapterDesc& DescIn)
:OwningRHI(nullptr)
,Desc(DescIn)
,Device(nullptr)
,Fence(nullptr)
{
}

void FD3D12Adapter::ShutDown()
{
	if(Device)
	{
		Device->ShutDown();
		delete Device;
		Device = nullptr;
	}

	if(Fence)
	{
		Fence->ShutDown();
		delete Fence;
		Fence = nullptr;
	}
	RtvHeap.Reset();
	DsvHeap.Reset();
	CbvSrvHeap.Reset();
	SamplerHeap.Reset();
	SwapChain.Reset();
	RootSignature.Reset();
	for (size_t i = 0; i < PiplelineStateCache.size(); ++i)
	{
		PiplelineStateCache[i].Reset();
	}
	D3DDevice.Reset();
}

void FD3D12Adapter::Initialize(D3D12DynamicRHI* RHI)
{
	OwningRHI = RHI;
	InitializeDevices();
	CreateDescriptorHeaps();
	CreateSignature();
	CreateSampler();
}

void FD3D12Adapter::InitializeDevices()
{

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
		
		Device = new FD3D12Device(this);
		Device->Initialize();

		Fence = new FD3D12Fence("GPUFence", this);
		Fence->Initialize();
	}
}

void FD3D12Adapter::CreateDescriptorHeaps()
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

void FD3D12Adapter::CreateSampler()
{
	D3D12_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 1;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	GetD3DDevice()->CreateSampler(&SamplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart());
}

void FD3D12Adapter::CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flag, UINT NodeMask, REFIID riid,
	_COM_Outptr_  void** ppvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
	Desc.NumDescriptors = NumDescripters;
	Desc.Type = Type;
	Desc.Flags = Flag;
	Desc.NodeMask = NodeMask;
	ThrowIfFailed(GetD3DDevice()->CreateDescriptorHeap(&Desc, riid, ppvHeap));
}

void FD3D12Adapter::SetViewPort(const FRHIViewPort& InViewPort)
{
	ViewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, InViewPort.Width, InViewPort.Height);
}

void FD3D12Adapter::SetScissorRect(const FRHIScissorRect& InRect)
{
	ScissorRect = CD3DX12_RECT(InRect.Left, InRect.Top, InRect.Right, InRect.Bottom);
}

void FD3D12Adapter::CreateSwapChain(const FRHISwapObjectInfo& SwapInfo)
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

	//rtv
	CreateDescripterHeap(SwapInfo.ObjectCount, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&RtvHeap));
	NAME_D3D12_OBJECT(RtvHeap);
}

void FD3D12Adapter::CreateSignature()
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

void FD3D12Adapter::CreatePso(const FRHIPiplineStateInitializer& PsoInitializer)
{
	CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
	rasterizerStateDesc.FrontCounterClockwise = true;
	rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;
	auto ConvertRHIFormatToD3DFormat = [](ERHI_DATA_FORMAT InFormat) -> DXGI_FORMAT
	{
		switch(InFormat)
		{
		case ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT: { return DXGI_FORMAT_R32G32B32_FLOAT;}
		case ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT:{ return DXGI_FORMAT_R32G32_FLOAT;}
		default: {return DXGI_FORMAT_UNKNOWN;}
		}
	};

	auto ConvertRHIClassificationToD3D = [](ERHI_INPUT_CLASSIFICATION InClassify) ->D3D12_INPUT_CLASSIFICATION
	{
		switch (InClassify)
		{
		case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; }
		case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_INSTANCE_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA; }
		default: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;}
		}
	};

	for(UINT i = 0; i < PsoInitializer.NumElements; i++)
	{
		FRHIInputElement* RHIDesc = (FRHIInputElement*)(PsoInitializer.pInpueElement + i);
		D3D12_INPUT_ELEMENT_DESC D3DInputDesc;
		D3DInputDesc.SemanticName = RHIDesc->SemanticName.c_str();
		D3DInputDesc.SemanticIndex = RHIDesc->SemanticIndex;
		D3DInputDesc.Format = ConvertRHIFormatToD3DFormat(RHIDesc->Format);
		D3DInputDesc.InputSlot = RHIDesc->InputSlot;
		D3DInputDesc.AlignedByteOffset = RHIDesc->AlignedByteOffset;
		D3DInputDesc.InputSlotClass = ConvertRHIClassificationToD3D(RHIDesc->InputSlotClass);
		D3DInputDesc.InstanceDataStepRate = RHIDesc->InstanceDataStepRate;
		InputElementDescs.push_back(D3DInputDesc);
	}

	PsoDesc.InputLayout = { InputElementDescs.data(), PsoInitializer.NumElements };
	PsoDesc.pRootSignature = GetRootSignature();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(PsoInitializer.pVSPointer, PsoInitializer.VsPointerLength);
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(PsoInitializer.pPsPointer, PsoInitializer.PsPointerLength);
	PsoDesc.RasterizerState = rasterizerStateDesc;
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = PsoInitializer.NumRenderTargets;
	PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	ComPtr<ID3D12PipelineState> PipelineState;
	ThrowIfFailed(GetD3DDevice()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PipelineState)));
	NAME_D3D12_OBJECT(PipelineState);
	PiplelineStateCache.push_back(PipelineState);
	
}