
#include "FD3D12Adapter.h"
#include "FD3D12Helper.h"
#include "FD3D12Resource.h"
#include "d3dx12.h"
#include "FDefine.h"

FD3D12Adapter::FD3D12Adapter(FD3D12AdapterDesc& DescIn)
:OwningRHI(nullptr)
,Desc(DescIn)
,CommandListManager(nullptr)
, FenceValue(0)
{
}

void FD3D12Adapter::ShutDown()
{
	WaitForPreviousFrame();
	CloseHandle(FenceEvent);
	GpuFence.Reset();
	RtvHeap.Reset();
	DsvHeap.Reset();
	CbvSrvHeap.Reset();
	SamplerHeap.Reset();
	DepthStencilBuffer.Reset();
	SwapChain.Reset();
	RootSignature.Reset();

	DefaultPso.Reset();
	for (auto Pso : PiplelineStateCache)
	{
		Pso.second.Reset();
	}
	PiplelineStateCache.clear();

	for (size_t i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		if(RenderTargets)
			RenderTargets[i].Reset();
	}

	for (auto Cb : ConstantBuffers)
	{
		Cb.second->Destroy();
	}
	ConstantBuffers.clear();

	if (CommandListManager)
	{
		CommandListManager->Clear();
		CommandListManager = nullptr;
	}
	D3DDevice.Reset();
}

void FD3D12Adapter::Initialize(FD3D12DynamicRHI* RHI)
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

		CommandListManager = new FD3D12CommandListManager(this);
		CommandListManager->Initialize();
		CommandListManager->CreateCommandLists(COMMAND_LIST_NUM);
		
		ThrowIfFailed(GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GpuFence)));
		NAME_D3D12_OBJECT(GpuFence);
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	}
}

void FD3D12Adapter::CreateDescriptorHeaps()
{
	//dsv
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&DsvHeap));
	NAME_D3D12_OBJECT(DsvHeap);
	
	//sample
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&SamplerHeap));
	NAME_D3D12_OBJECT(SamplerHeap);

	RtvDescriptorSize = GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void FD3D12Adapter::CreateSampler()
{
	D3D12_SAMPLER_DESC SamplerDesc = {};
	SamplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
	SamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	SamplerDesc.MinLOD = 0;
	SamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	SamplerDesc.MipLODBias = 0.0f;
	SamplerDesc.MaxAnisotropy = 8;
	SamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
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

void FD3D12Adapter::InitWindow(UINT Width, UINT Height, void* InWindwow)
{
	WndWidth = Width;
	WndHeight = Height;
	Window = (HWND)InWindwow;
}

void FD3D12Adapter::SetViewPort(const FRHIViewPort& InViewPort)
{
	ViewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, InViewPort.Width, InViewPort.Height);
}

void FD3D12Adapter::SetScissorRect(const FRHIScissorRect& InRect)
{
	ScissorRect = CD3DX12_RECT(InRect.Left, InRect.Top, InRect.Right, InRect.Bottom);
}

void FD3D12Adapter::CreateSwapChain()
{	
	SwapChain.Reset();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC Sd;
	Sd.BufferDesc.Width = WndWidth;
	Sd.BufferDesc.Height = WndHeight;
	Sd.BufferDesc.RefreshRate.Numerator = 60;
	Sd.BufferDesc.RefreshRate.Denominator = 1;
	Sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	Sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	Sd.SampleDesc.Count = 1;
	Sd.SampleDesc.Quality = 0;
	Sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Sd.BufferCount = RENDER_TARGET_COUNT;
	Sd.OutputWindow = (HWND)Window;
	Sd.Windowed = true;
	Sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(DxgiFactory->CreateSwapChain(
		GetD3DCommandQueue(),
		&Sd,
		&SwapChain));

	//rtv
	CreateDescripterHeap(RENDER_TARGET_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&RtvHeap));
	NAME_D3D12_OBJECT(RtvHeap);
}

void FD3D12Adapter::CreateSignature()
{
	CD3DX12_ROOT_PARAMETER RootParameters[5];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[5];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	Ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	Ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	Ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);


	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[1].InitAsDescriptorTable(1, &Ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[2].InitAsDescriptorTable(1, &Ranges[2], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[3].InitAsDescriptorTable(1, &Ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameters[4].InitAsDescriptorTable(1, &Ranges[4], D3D12_SHADER_VISIBILITY_PIXEL);

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(RootParameters), RootParameters, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> Signature;
	ComPtr<ID3DBlob> Error;
	
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
	ThrowIfFailed(GetD3DDevice()->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
	NAME_D3D12_OBJECT(RootSignature);
}

void FD3D12Adapter::CreatePso(const FRHIPiplineStateInitializer& PsoInitializer, bool bDefaultPso)
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
		case ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT: { return DXGI_FORMAT_R32G32B32_FLOAT; }
		case ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT:{ return DXGI_FORMAT_R32G32_FLOAT; }
		case ERHI_DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT: { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
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
	PiplelineStateCache[PsoInitializer.KeyName] = PipelineState;
	if(bDefaultPso)
	{
		DefaultPso = PipelineState;
		NAME_D3D12_OBJECT(PipelineState);
	}
}

void FD3D12Adapter::CreateSrvAndCbvs(FCbvSrvDesc Desc)
{
	CurrentCbvSrvDesc = Desc;

	CreateDescripterHeap(CurrentCbvSrvDesc.NeedDesciptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&CbvSrvHeap));
	NAME_D3D12_OBJECT(CbvSrvHeap);
	CbvSrvDescriptorSize = GetD3DDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//create matrix constant buffer
	FD3DConstantBuffer* MtCb = new FD3DConstantBuffer();
	MtCb->Initialize();
	MtCb->SetConstantBufferInfo(this, CurrentCbvSrvDesc.CbMatrix.BufferSize);
	ConstantBuffers[CurrentCbvSrvDesc.CbMatrix.BufferType] = MtCb;
	
	CD3DX12_CPU_DESCRIPTOR_HANDLE Cbv1Handle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbMatrix.HeapOffsetStart, 
		CbvSrvDescriptorSize);

	UINT64 CbOffset = 0;
	for (UINT i = 0; i < CurrentCbvSrvDesc.CbMatrix.BufferViewCount; i++)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
		CbvDesc.BufferLocation = MtCb->GetD3DConstantBuffer()->GetGPUVirtualAddress() + CbOffset;
		CbvDesc.SizeInBytes = CurrentCbvSrvDesc.CbMatrix.BufferSize / CurrentCbvSrvDesc.CbMatrix.BufferViewCount;
		CbOffset += CbvDesc.SizeInBytes;
		GetD3DDevice()->CreateConstantBufferView(&CbvDesc, Cbv1Handle);
		Cbv1Handle.Offset(CbvSrvDescriptorSize);
	}

	//create material constant buffer
	FD3DConstantBuffer* MatCb = new FD3DConstantBuffer();
	MatCb->Initialize();
	MatCb->SetConstantBufferInfo(this, CurrentCbvSrvDesc.CbMaterial.BufferSize);
	ConstantBuffers[CurrentCbvSrvDesc.CbMaterial.BufferType] = MatCb;

	CD3DX12_CPU_DESCRIPTOR_HANDLE Cbv2Handle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbMaterial.HeapOffsetStart,
		CbvSrvDescriptorSize);
	CbOffset = 0;
	for (UINT i = 0; i < CurrentCbvSrvDesc.CbMaterial.BufferViewCount; i++)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Cbv2Desc = {};
		Cbv2Desc.BufferLocation = MatCb->GetD3DConstantBuffer()->GetGPUVirtualAddress() + CbOffset;
		Cbv2Desc.SizeInBytes = CurrentCbvSrvDesc.CbMaterial.BufferSize / CurrentCbvSrvDesc.CbMaterial.BufferViewCount;
		CbOffset += Cbv2Desc.SizeInBytes;
		GetD3DDevice()->CreateConstantBufferView(&Cbv2Desc, Cbv2Handle);
		Cbv2Handle.Offset(CbvSrvDescriptorSize);
	}

	//create pass constant
	FD3DConstantBuffer* PassConstantCb = new FD3DConstantBuffer();
	PassConstantCb->Initialize();
	PassConstantCb->SetConstantBufferInfo(this, CurrentCbvSrvDesc.CbConstant.BufferSize);
	ConstantBuffers[CurrentCbvSrvDesc.CbConstant.BufferType] =  PassConstantCb;
	CD3DX12_CPU_DESCRIPTOR_HANDLE Cbv3Handle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbConstant.HeapOffsetStart,
		CbvSrvDescriptorSize);

	D3D12_CONSTANT_BUFFER_VIEW_DESC Cbv3Desc = {};
	Cbv3Desc.BufferLocation = PassConstantCb->GetD3DConstantBuffer()->GetGPUVirtualAddress();
	Cbv3Desc.SizeInBytes = CurrentCbvSrvDesc.CbConstant.BufferSize ;
	GetD3DDevice()->CreateConstantBufferView(&Cbv3Desc, Cbv3Handle);

	//CurrentCbvSrvDesc.SrvDesc  can use to create texture in mesh, use GetCurrentCbvSrvDesc()
}


void FD3D12Adapter::CreateRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	IDXGISwapChain* SwapChain = GetSwapChain();
	// Create a RTV for each frame.
	for (UINT n = 0; n < RENDER_TARGET_COUNT; n++)
	{
		ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n])));
		GetD3DDevice()->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RtvHandle);
		RtvHandle.Offset(1, RtvDescriptorSize);
	}

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES ProDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndWidth, WndHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	ThrowIfFailed(GetD3DDevice()->CreateCommittedResource(
		&ProDefault,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&DepthStencilBuffer)
	));
	NAME_D3D12_OBJECT(DepthStencilBuffer);

	// Create the depth/stencil buffer and view.
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
	GetD3DDevice()->CreateDepthStencilView(DepthStencilBuffer.Get(), &depthStencilDesc, DsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void FD3D12Adapter::InitRenderBegin(ID3D12GraphicsCommandList* CommandList, UINT TargetFrame, FRHIColor Color)
{
	const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[TargetFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &Rb1);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetFrame, RtvDescriptorSize);

	float ClearColor[4] = { Color.R, Color.G, Color.B, Color.A };
	CommandList->ClearRenderTargetView(RtvHandle, ClearColor, 0, nullptr);
	CommandList->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);
}

void FD3D12Adapter::RenderEnd(ID3D12GraphicsCommandList* CommandList, UINT TargetFrame)
{
	const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[TargetFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(1, &Rb2);
}

void FD3D12Adapter::WaitForPreviousFrame()
{
	const UINT64 Value = FenceValue;
	ThrowIfFailed(GetD3DCommandQueue()->Signal(GpuFence.Get(), Value));
	FenceValue ++;
	if(GpuFence->GetCompletedValue() < Value)
	{
		ThrowIfFailed(GpuFence->SetEventOnCompletion(Value, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}