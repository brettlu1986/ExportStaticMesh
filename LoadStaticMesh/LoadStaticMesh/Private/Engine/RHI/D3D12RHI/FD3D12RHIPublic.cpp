
#include "stdafx.h"

#include <dxgidebug.h>
#include <d3dcompiler.h>
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FDefine.h"
#include "FD3D12Resource.h"
#include "LAssetDataLoader.h"
#include "LEngine.h"
#include "d3dx12.h"
#include "LDeviceWindows.h"


using namespace Microsoft::WRL;
using namespace DirectX;



FDynamicRHI* FD3D12DynamicRHIModule::CreateRHI()
{
	return new FD3D12DynamicRHI();
}

/////////////////////////// dynamic RHI implement
FD3D12DynamicRHI::FD3D12DynamicRHI()
{
	
}

void FD3D12DynamicRHI::FindAdapter()
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
	CurrentAdapterDesc = FD3D12AdapterDesc(Desc, Index, MaxSupportedFeatureLevel, pDevice->GetNodeCount());
	pDevice->Release();
}


void FD3D12DynamicRHI::Init() 
{
	FindAdapter();
	InitializeDevices();
	CreateDescriptorHeaps();
	CreateSignature();
	CreateSampler();
	LDeviceWindows* DeviceWindows = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	InitWindow(DeviceWindows->GetWidth(), DeviceWindows->GetHeight(), DeviceWindows->GetHwnd());
}

void FD3D12DynamicRHI::InitializeDevices()
{
	if (!D3DDevice)
	{
		ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));

		ComPtr<IDXGIFactory6> Factory6;
		if (SUCCEEDED(DxgiFactory->QueryInterface(IID_PPV_ARGS(&Factory6))))
		{
			Factory6->EnumAdapterByGpuPreference(
				CurrentAdapterDesc.AdapterIndex,
				DXGI_GPU_PREFERENCE_UNSPECIFIED,
				IID_PPV_ARGS(&DxgiAdapter));
		}

		HRESULT HardwareResult = D3D12CreateDevice(
			DxgiAdapter.Get(),             // default adapter
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&D3DDevice));

		if (FAILED(HardwareResult))
		{
			ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&DxgiAdapter)));
			ThrowIfFailed(D3D12CreateDevice(
				DxgiAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(&D3DDevice)));
		}
		NAME_D3D12_OBJECT(D3DDevice);

		D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
		QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		ThrowIfFailed(D3DDevice.Get()->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
		NAME_D3D12_OBJECT(CommandQueue);

		ThrowIfFailed(D3DDevice.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
		NAME_D3D12_OBJECT_WITHINDEX(CommandAllocator, 0);

		ThrowIfFailed(D3DDevice.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&CommandList)));
		NAME_D3D12_OBJECT(CommandList);

		ThrowIfFailed(D3DDevice.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GpuFence)));
		NAME_D3D12_OBJECT(GpuFence);
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	}
}

void FD3D12DynamicRHI::CreateDescriptorHeaps()
{
	//dsv
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&DsvHeap));
	NAME_D3D12_OBJECT(DsvHeap);

	//sample
	CreateDescripterHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&SamplerHeap));
	NAME_D3D12_OBJECT(SamplerHeap);

	RtvDescriptorSize = D3DDevice.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = D3DDevice.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void FD3D12DynamicRHI::CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flag, UINT NodeMask, REFIID riid,
	_COM_Outptr_  void** ppvHeap)
{
	D3D12_DESCRIPTOR_HEAP_DESC Desc = {};
	Desc.NumDescriptors = NumDescripters;
	Desc.Type = Type;
	Desc.Flags = Flag;
	Desc.NodeMask = NodeMask;
	ThrowIfFailed(D3DDevice.Get()->CreateDescriptorHeap(&Desc, riid, ppvHeap));
}

void FD3D12DynamicRHI::CreateSignature()
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
	ThrowIfFailed(D3DDevice.Get()->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
	NAME_D3D12_OBJECT(RootSignature);
}

void FD3D12DynamicRHI::CreateSampler()
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
	D3DDevice.Get()->CreateSampler(&SamplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart());
}


void FD3D12DynamicRHI::InitWindow(UINT Width, UINT Height, void* InWindow)
{
	WndWidth = Width;
	WndHeight = Height;
	Window = (HWND)InWindow;

	ViewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height));
	ScissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height));
	
	CreateSwapChain();
	CreateRenderTargets();
}

void FD3D12DynamicRHI::CreateSwapChain()
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
		CommandQueue.Get(),
		&Sd,
		&SwapChain));

	//rtv
	CreateDescripterHeap(RENDER_TARGET_COUNT, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&RtvHeap));
	NAME_D3D12_OBJECT(RtvHeap);
}

void FD3D12DynamicRHI::CreateRenderTargets()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	// Create a RTV for each frame.
	for (UINT n = 0; n < RENDER_TARGET_COUNT; n++)
	{
		ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n])));
		D3DDevice.Get()->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RtvHandle);
		RtvHandle.Offset(1, RtvDescriptorSize);
	}

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES ProDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndWidth, WndHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	ThrowIfFailed(D3DDevice.Get()->CreateCommittedResource(
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
	D3DDevice.Get()->CreateDepthStencilView(DepthStencilBuffer.Get(), &depthStencilDesc, DsvHeap->GetCPUDescriptorHandleForHeapStart());
}

void FD3D12DynamicRHI::ShutDown()
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
		if (RenderTargets)
			RenderTargets[i].Reset();
	}

	for (auto Cb : ConstantBuffers)
	{
		Cb.second->Destroy();
	}
	ConstantBuffers.clear();

	CommandAllocator.Reset();
	CommandList.Reset();
	CommandQueue.Reset();

	D3DDevice.Reset();

//#if defined(_DEBUG)
//	{
//		ComPtr<IDXGIDebug1> DxgiDebug;
//		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
//		{
//			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
//		}
//	}
//#endif
}
 
 void FD3D12DynamicRHI::RHICreateSrvAndCbvs(FCbvSrvDesc Desc)
 {
	 CurrentCbvSrvDesc = Desc;

	 CreateDescripterHeap(CurrentCbvSrvDesc.NeedDesciptorCount, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		 D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0, IID_PPV_ARGS(&CbvSrvHeap));
	 NAME_D3D12_OBJECT(CbvSrvHeap);
	 CbvSrvDescriptorSize = D3DDevice.Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	 //create matrix constant buffer
	 FD3DConstantBuffer* MtCb = new FD3DConstantBuffer();
	 MtCb->Initialize();
	 MtCb->SetConstantBufferInfo(D3DDevice.Get(), CurrentCbvSrvDesc.CbMatrix.BufferSize);
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
		 D3DDevice.Get()->CreateConstantBufferView(&CbvDesc, Cbv1Handle);
		 Cbv1Handle.Offset(CbvSrvDescriptorSize);
	 }

	 //create material constant buffer
	 FD3DConstantBuffer* MatCb = new FD3DConstantBuffer();
	 MatCb->Initialize();
	 MatCb->SetConstantBufferInfo(D3DDevice.Get(), CurrentCbvSrvDesc.CbMaterial.BufferSize);
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
		 D3DDevice.Get()->CreateConstantBufferView(&Cbv2Desc, Cbv2Handle);
		 Cbv2Handle.Offset(CbvSrvDescriptorSize);
	 }

	 //create pass constant
	 FD3DConstantBuffer* PassConstantCb = new FD3DConstantBuffer();
	 PassConstantCb->Initialize();
	 PassConstantCb->SetConstantBufferInfo(D3DDevice.Get(), CurrentCbvSrvDesc.CbConstant.BufferSize);
	 ConstantBuffers[CurrentCbvSrvDesc.CbConstant.BufferType] = PassConstantCb;
	 CD3DX12_CPU_DESCRIPTOR_HANDLE Cbv3Handle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbConstant.HeapOffsetStart,
		 CbvSrvDescriptorSize);

	 D3D12_CONSTANT_BUFFER_VIEW_DESC Cbv3Desc = {};
	 Cbv3Desc.BufferLocation = PassConstantCb->GetD3DConstantBuffer()->GetGPUVirtualAddress();
	 Cbv3Desc.SizeInBytes = CurrentCbvSrvDesc.CbConstant.BufferSize;
	 D3DDevice.Get()->CreateConstantBufferView(&Cbv3Desc, Cbv3Handle);
 }

 void FD3D12DynamicRHI::RHIUpdateConstantBuffer(void* pUpdateData)
 {
	FBufferObject* BuffObj = (FBufferObject*)(pUpdateData);
	FD3DConstantBuffer* Cb = ConstantBuffers[BuffObj->Type];
	if(Cb)
	{
		Cb->UpdateConstantBufferInfo(BuffObj->BufferData, BuffObj->DataSize);
	}
 }

 FShader* FD3D12DynamicRHI::RHICreateShader(LPCWSTR  ShaderFile)
 {
	 UINT8* ShaderData = nullptr;
	 UINT ShaderLen = 0;
	 ThrowIfFailed(ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(ShaderFile).c_str(), &ShaderData, &ShaderLen));
	 return new FShader(ShaderData, ShaderLen);
 }

 void FD3D12DynamicRHI::RHICreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey, bool bDefaultPso)
 {
	 FRHIPiplineStateInitializer RHIPsoInitializer = {
		PsoKey,
		StandardInputElementDescs,
		_countof(StandardInputElementDescs),
		Vs->GetShaderByteCode(),
		Vs->GetDataLength(),
		Ps->GetShaderByteCode(),
		Ps->GetDataLength(),
		1
	 };

	 CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
	 rasterizerStateDesc.FrontCounterClockwise = true;
	 rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

	 D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	 ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	 std::vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;
	 auto ConvertRHIFormatToD3DFormat = [](ERHI_DATA_FORMAT InFormat) -> DXGI_FORMAT
	 {
		 switch (InFormat)
		 {
		 case ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT: { return DXGI_FORMAT_R32G32B32_FLOAT; }
		 case ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT: { return DXGI_FORMAT_R32G32_FLOAT; }
		 case ERHI_DATA_FORMAT::FORMAT_R32G32B32A32_FLOAT: { return DXGI_FORMAT_R32G32B32A32_FLOAT; }
		 default: {return DXGI_FORMAT_UNKNOWN; }
		 }
	 };

	 auto ConvertRHIClassificationToD3D = [](ERHI_INPUT_CLASSIFICATION InClassify) ->D3D12_INPUT_CLASSIFICATION
	 {
		 switch (InClassify)
		 {
		 case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; }
		 case ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_INSTANCE_DATA: { return D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA; }
		 default: { return D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA; }
		 }
	 };

	 for (UINT i = 0; i < RHIPsoInitializer.NumElements; i++)
	 {
		 FRHIInputElement* RHIDesc = (FRHIInputElement*)(RHIPsoInitializer.pInpueElement + i);
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

	 PsoDesc.InputLayout = { InputElementDescs.data(), RHIPsoInitializer.NumElements };
	 PsoDesc.pRootSignature = RootSignature.Get();
	 PsoDesc.VS = CD3DX12_SHADER_BYTECODE(RHIPsoInitializer.pVSPointer, RHIPsoInitializer.VsPointerLength);
	 PsoDesc.PS = CD3DX12_SHADER_BYTECODE(RHIPsoInitializer.pPsPointer, RHIPsoInitializer.PsPointerLength);
	 PsoDesc.RasterizerState = rasterizerStateDesc;
	 PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	 PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	 PsoDesc.SampleMask = UINT_MAX;
	 PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	 PsoDesc.NumRenderTargets = RHIPsoInitializer.NumRenderTargets;
	 PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	 PsoDesc.SampleDesc.Count = 1;
	 PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	 PsoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	 ComPtr<ID3D12PipelineState> PipelineState;
	 ThrowIfFailed(D3DDevice.Get()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PipelineState)));
	 NAME_D3D12_OBJECT(PipelineState);
	 PiplelineStateCache[RHIPsoInitializer.KeyName] = PipelineState;
	 if (bDefaultPso)
	 {
		 DefaultPso = PipelineState;
		 NAME_D3D12_OBJECT(PipelineState);
	 }
		
 }

 void FD3D12DynamicRHI::RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color)
 {

	 CommandAllocator->Reset();
	 CommandList->Reset(CommandAllocator.Get(), DefaultPso.Get());

	 
	 CommandList->SetGraphicsRootSignature(RootSignature.Get());

	 ID3D12DescriptorHeap* ppHeaps[] = { CbvSrvHeap.Get(), SamplerHeap.Get() };
	 CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	 CommandList->RSSetViewports(1, &(ViewPort));
	 CommandList->RSSetScissorRects(1, &(ScissorRect));


	 const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[TargetFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	 CommandList->ResourceBarrier(1, &Rb1);
	 CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), TargetFrame, RtvDescriptorSize);
	 float ClearColor[4] = { Color.R, Color.G, Color.B, Color.A };
	 CommandList->ClearRenderTargetView(RtvHandle, ClearColor, 0, nullptr);
	 CommandList->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	 D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	 CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);

	 // pass constant buffer
	 CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbConstant.HeapOffsetStart,
		 CbvSrvDescriptorSize);
	 CommandList->SetGraphicsRootDescriptorTable(2, cbvSrvHandle);
 }

 void FD3D12DynamicRHI::RHIDrawMesh(FMesh* Mesh)
 {
	

	 FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(Mesh->GetIndexBuffer());
	 FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(Mesh->GetVertexBuffer());

	 CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	 CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
	 CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	 CommandList->SetPipelineState(PiplelineStateCache[Mesh->GetPsoKey()].Get());

	
	 CD3DX12_GPU_DESCRIPTOR_HANDLE MtHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbMatrix.HeapOffsetStart + Mesh->GetBufferIndex(),
		 CbvSrvDescriptorSize);
	 CommandList->SetGraphicsRootDescriptorTable(0, MtHandle);

	 FMaterial* Material = Mesh->GetMaterial();
	 CD3DX12_GPU_DESCRIPTOR_HANDLE MatHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbMaterial.HeapOffsetStart + Material->GetMaterialCbvHeapIndex(),
		 CbvSrvDescriptorSize);
	 CommandList->SetGraphicsRootDescriptorTable(1, MatHandle);

	 if(Mesh->GetDiffuseTexture())
	 {
		 FTexture* Tex = Mesh->GetDiffuseTexture();
		 CD3DX12_GPU_DESCRIPTOR_HANDLE TexHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.SrvDesc.HeapOffsetStart + Tex->GetTextureHeapIndex(),
			 CbvSrvDescriptorSize);
		 CommandList->SetGraphicsRootDescriptorTable(3, TexHandle);
		 CommandList->SetGraphicsRootDescriptorTable(4, SamplerHeap->GetGPUDescriptorHandleForHeapStart());
	 }

	 CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
 }


 void FD3D12DynamicRHI::RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute)
 {
	 
	 std::vector<ID3D12CommandList*> CmdLists;
	 if(bFirstExcute)
	 {
		 CommandList->Close();
		 CmdLists.push_back(CommandList.Get());
		 CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
		 SetFenceValue(1);
	 }
	 else 
	 {	
		 const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[TargetFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		 CommandList->ResourceBarrier(1, &Rb2);
		 CommandList->Close();
		 CmdLists.push_back(CommandList.Get());
		 CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
		 SwapChain->Present(1, 0);
	 }
	 WaitForPreviousFrame();
 }


 FIndexBuffer* FD3D12DynamicRHI::RHICreateIndexBuffer()
 {
	return new FD3D12IndexBuffer();
 }

 FVertexBuffer* FD3D12DynamicRHI::RHICreateVertexBuffer()
 {
	return new FD3D12VertexBuffer();
 }

 FTexture* FD3D12DynamicRHI::RHICreateTexture()
 {
	return new FD3D12Texture();
 }

void FD3D12DynamicRHI::RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(D3DDevice.Get(), CommandList.Get());
	D3DVertexBuffer->InitGPUVertexBufferView(D3DDevice.Get(), CommandList.Get());
	if(Texture)
	{
		FD3D12Texture* D3DTexture = dynamic_cast<FD3D12Texture*>(Texture);
		D3DTexture->InitGPUTextureView(D3DDevice.Get(), CommandList.Get(), CbvSrvDescriptorSize, CbvSrvHeap.Get(), CurrentCbvSrvDesc);
	}
}

void FD3D12DynamicRHI::WaitForPreviousFrame()
{
	const UINT64 Value = FenceValue;
	ThrowIfFailed(CommandQueue->Signal(GpuFence.Get(), Value));
	FenceValue++;
	if (GpuFence->GetCompletedValue() < Value)
	{
		ThrowIfFailed(GpuFence->SetEventOnCompletion(Value, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}


