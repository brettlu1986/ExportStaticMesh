

#include "stdafx.h"
#include "GraphicRenderModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <iostream>
#include "ApplicationMain.h"
#include "d3dx12.h"
#include "DDSTextureLoader.h"

using namespace Microsoft::WRL;
using namespace DirectX;

static const wstring TexFileName = L"T_Chair_M.dds";

GraphicRenderModel* GraphicRenderModel::ThisRenderModel = nullptr;

GraphicRenderModel::GraphicRenderModel()
	: FrameIndex(0)
	, RtvDescriptorSize(0)
	, FenceValue(0)
	, CbvSrvUavDescriptorSize(0)
	, DsvDescriptorSize(0)
	, pCbvDataBegin(nullptr)
	, ObjectConstant({})
	, ConstantBufferSize(0)
	, State(0)
	
{
	MtProj = MathHelper::Identity4x4();
	LastMousePoint = {0 , 0};
	ThisRenderModel = this;
}

GraphicRenderModel::~GraphicRenderModel()
{

}

void GraphicRenderModel::OnInit()
{
	LoadPipline();
	LoadAssets();
	CreateRenderThread();
}

void GraphicRenderModel::LoadPipline()
{
	CreateDxgiObjects();
	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
}

void GraphicRenderModel::LoadAssets()
{
	CreateRootSignature();
	LoadShadersAndCreatePso();
}

void GraphicRenderModel::CreateDxgiObjects()
{
	Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(WndWidth), static_cast<float>(WndHeight));
	ScissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(WndWidth), static_cast<LONG>(WndHeight));

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
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));
	// Try to create hardware device.
	HRESULT HardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&Device));

	if (FAILED(HardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(DxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&Device)));
	}
	NAME_D3D12_OBJECT(Device);

	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	NAME_D3D12_OBJECT(Fence);
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicRenderModel::CreateCommandObjects()
{
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	NAME_D3D12_OBJECT(CommandQueue);

	//CommandListPre 
	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocatorPre)));
	NAME_D3D12_OBJECT(CommandAllocatorPre);
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocatorPre.Get(), nullptr, IID_PPV_ARGS(&CommandListPre)));
	NAME_D3D12_OBJECT(CommandListPre);

	//CommandListPost 
	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocatorPost)));
	NAME_D3D12_OBJECT(CommandAllocatorPost);
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocatorPost.Get(), nullptr, IID_PPV_ARGS(&CommandListPost)));
	NAME_D3D12_OBJECT(CommandListPost);

	ThrowIfFailed(CommandListPre->Close());
	ThrowIfFailed(CommandListPost->Close());
}

void GraphicRenderModel::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
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
	Sd.SampleDesc.Count =  1;
	Sd.SampleDesc.Quality =  0;
	Sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Sd.BufferCount = FrameCount;
	Sd.OutputWindow = MainApplication->GetHwnd();
	Sd.Windowed = true;
	Sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(DxgiFactory->CreateSwapChain(
		CommandQueue.Get(),       
		&Sd,
		&SwapChain));

}

void GraphicRenderModel::CreateDescriptorHeaps()
{
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC RtvHeapDesc = {};
		RtvHeapDesc.NumDescriptors = FrameCount;
		RtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(Device->CreateDescriptorHeap(&RtvHeapDesc, IID_PPV_ARGS(&RtvHeap)));
		NAME_D3D12_OBJECT(RtvHeap);

		D3D12_DESCRIPTOR_HEAP_DESC DsvHeapDesc = {};
		DsvHeapDesc.NumDescriptors = 1;
		DsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		DsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		DsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device->CreateDescriptorHeap(
			&DsvHeapDesc, IID_PPV_ARGS(&DsvHeap)));
		NAME_D3D12_OBJECT(DsvHeap);
		
		D3D12_DESCRIPTOR_HEAP_DESC CbvSrvHeapDesc = {};
		CbvSrvHeapDesc.NumDescriptors = 2;//constant buffer and shader resource view buffer
		CbvSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		CbvSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		CbvSrvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device->CreateDescriptorHeap(&CbvSrvHeapDesc,
			IID_PPV_ARGS(&CbvSrvHeap)));
		NAME_D3D12_OBJECT(CbvSrvHeap);

		// Describe and create a sampler descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC SamplerHeapDesc = {};
		SamplerHeapDesc.NumDescriptors = 1;
		SamplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		SamplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowIfFailed(Device->CreateDescriptorHeap(&SamplerHeapDesc, IID_PPV_ARGS(&SamplerHeap)));
		NAME_D3D12_OBJECT(SamplerHeap);
	}
}

void GraphicRenderModel::CreateRootSignature()
{
	// Create a root Signature consisting of a descriptor table with a single CBV.
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
	ThrowIfFailed(Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
	NAME_D3D12_OBJECT(RootSignature);

}

void GraphicRenderModel::LoadShadersAndCreatePso()
{
	// Create the pipeline state, which includes compiling and loading shaders.
	ComPtr<ID3DBlob> pVertexShader;
	ComPtr<ID3DBlob> pPixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> ErrorsVs, ErrorsPs;
	D3DCompileFromFile(GetAssetFullPath(L"shader_vs.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &pVertexShader, &ErrorsVs);
	D3DCompileFromFile(GetAssetFullPath(L"shader_ps.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pPixelShader, &ErrorsPs);
	if (ErrorsVs != nullptr)
	{
		OutputDebugStringA((char*)ErrorsVs->GetBufferPointer());
	}
	if (ErrorsPs != nullptr)
	{
		OutputDebugStringA((char*)ErrorsPs->GetBufferPointer());
	}

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC InputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	  0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
	rasterizerStateDesc.FrontCounterClockwise = true;
	rasterizerStateDesc.CullMode = D3D12_CULL_MODE_NONE;

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PsoDesc.InputLayout = { InputElementDescs, _countof(InputElementDescs) };
	PsoDesc.pRootSignature = RootSignature.Get();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShader.Get());
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShader.Get());
	PsoDesc.RasterizerState = rasterizerStateDesc;
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = 1;
	PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	ThrowIfFailed(Device->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PipelineState)));
	NAME_D3D12_OBJECT(PipelineState);

	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
	// Create a RTV for each frame.
	for (UINT n = 0; n < FrameCount; n++)
	{
		ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n])));
		Device->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RtvHandle);
		RtvHandle.Offset(1, RtvDescriptorSize);
	}
}

//------------- render thread

void GraphicRenderModel::CreateRenderThread()
{
	struct ThreadWrapper
	{
		static unsigned int WINAPI Thunk(void* LParameter)
		{
			GraphicRenderModel::Get()->RenderThread(LParameter);
			return 0;
		}
	};

	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&RenderThreadCmdAllocator)));
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, RenderThreadCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&RenderThreadCmdList)));
	NAME_D3D12_OBJECT(RenderThreadCmdAllocator);
	NAME_D3D12_OBJECT(RenderThreadCmdList);

	ThreadParam.RenderBegin = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	ThreadParam.RenderRun = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	ThreadParam.RenderEnd = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	HWaitedArr.push_back(ThreadParam.RenderBegin);

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

UINT GraphicRenderModel::RenderThread(void* Param)
{
	RenderThreadInit();
	SetEvent(ThreadParam.RenderBegin);
	RenderThreadRun();
	return 0;
}

void GraphicRenderModel::RenderThreadInit()
{
	InitCamera();
	InitModel();
	CreateConstantBufferView();
	CreateDepthStencilView();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateTextureAndSampler();
}

void GraphicRenderModel::RenderThreadRun()
{
	DWORD Ret = 0;
	while (!bDestroy)
	{
		//update matrix
		XMMATRIX WorldViewProj = ModelGeo.GetModelMatrix() * Camera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
		//Update the constant buffer with the latest WorldViewProj matrix.
		XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
		memcpy(pCbvDataBegin, &ObjectConstant, sizeof(ObjectConstant));

		Ret = WaitForSingleObject(ThreadParam.RenderRun, INFINITE);
		if (Ret - WAIT_OBJECT_0 == 0)
		{
			RenderThreadCmdAllocator->Reset();
			RenderThreadCmdList->Reset(RenderThreadCmdAllocator.Get(), PipelineState.Get());

			//clear back buffer and depth buffer
			CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RtvDescriptorSize);
			D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
			RenderThreadCmdList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);
			RenderThreadCmdList->RSSetViewports(1, &Viewport);
			RenderThreadCmdList->RSSetScissorRects(1, &ScissorRect);

			// Set necessary state.
			RenderThreadCmdList->SetGraphicsRootSignature(RootSignature.Get());
			RenderThreadCmdList->SetPipelineState(PipelineState.Get());

			ID3D12DescriptorHeap* ppHeaps[] = { CbvSrvHeap.Get(), SamplerHeap.Get() };
			RenderThreadCmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			RenderThreadCmdList->SetGraphicsRootDescriptorTable(0, CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
			CD3DX12_GPU_DESCRIPTOR_HANDLE tex(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
			tex.Offset(1, CbvSrvUavDescriptorSize);
			RenderThreadCmdList->SetGraphicsRootDescriptorTable(1, tex);
			RenderThreadCmdList->SetGraphicsRootDescriptorTable(2, SamplerHeap->GetGPUDescriptorHandleForHeapStart());

			RenderThreadCmdList->IASetVertexBuffers(0, 1, &ModelGeo.GetVertexBufferView());
			RenderThreadCmdList->IASetIndexBuffer(&ModelGeo.GetIndexBufferView());
			RenderThreadCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			RenderThreadCmdList->DrawIndexedInstanced(ModelGeo.GetIndicesCount(), 1, 0, 0, 0);
			RenderThreadCmdList->Close();

			SetEvent(ThreadParam.RenderEnd);
		}
	}
}

void GraphicRenderModel::InitCamera()
{
	Camera.Init();
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	Camera.OnResize(static_cast<float>(WndWidth), static_cast<float>(WndHeight));
	XMMATRIX P = Camera.GetProjectionMatrix();
	XMStoreFloat4x4(&MtProj, P);
}

void GraphicRenderModel::InitModel()
{
	ModelGeo.Init();
	ModelGeo.SetModelLocation(Camera.GetViewTargetLocation());
}

void GraphicRenderModel::CreateConstantBufferView()
{
	//constant buffer size must 256's multiple
	ConstantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(ConstantBufferSize);

	ThrowIfFailed(Device->CreateCommittedResource(
		&ConstantProp,
		D3D12_HEAP_FLAG_NONE,
		&ConstantDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&ConstantBuffer)));

	ThrowIfFailed(ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCbvDataBegin)));
	memcpy(pCbvDataBegin, &ObjectConstant, sizeof(ObjectConstant));
	NAME_D3D12_OBJECT(ConstantBuffer);

	D3D12_CONSTANT_BUFFER_VIEW_DESC CbvDesc = {};
	CbvDesc.BufferLocation = ConstantBuffer->GetGPUVirtualAddress();
	CbvDesc.SizeInBytes = ConstantBufferSize;
	Device->CreateConstantBufferView(&CbvDesc, CbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicRenderModel::CreateDepthStencilView()
{
	// Create the depth/stencil buffer and view.
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES ProDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WndWidth, WndHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	ThrowIfFailed(Device->CreateCommittedResource(
		&ProDefault,
		D3D12_HEAP_FLAG_NONE,
		&ResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&DepthStencilBuffer)
	));

	NAME_D3D12_OBJECT(DepthStencilBuffer);
	Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &depthStencilDesc, DsvHeap->GetCPUDescriptorHandleForHeapStart());
}


void GraphicRenderModel::CreateVertexBufferView()
{
	std::vector<Vertex_PositionTex0> TriangleVertices;
	ModelGeo.GetPositionTex0Input(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(Vertex_PositionTex0));
	CreateBuffer(Device.Get(), RenderThreadCmdList.Get(), TriangleVertices.data(), VertexBufferSize, ModelGeo.VertexBuffer, ModelGeo.VertexUploadBuffer);
	ModelGeo.VertexBufferSize = VertexBufferSize;

	NAME_D3D12_OBJECT(ModelGeo.VertexBuffer);
	NAME_D3D12_OBJECT(ModelGeo.VertexUploadBuffer);

	ModelGeo.CreateVertexBufferView();
}

void GraphicRenderModel::CreateIndexBufferView()
{
	ModelGeo.IndicesCount = ModelGeo.bUseHalfInt32 ? 
		static_cast<UINT>(ModelGeo.MeshIndicesHalf.size()) : static_cast<UINT>(ModelGeo.MeshIndices.size());

	ModelGeo.IndiceSize = ModelGeo.bUseHalfInt32 ? 
		ModelGeo.IndicesCount * sizeof(UINT16) : ModelGeo.IndicesCount * sizeof(UINT) ;

	const void* InitData = ModelGeo.bUseHalfInt32 ? 
		reinterpret_cast<void*>(ModelGeo.MeshIndicesHalf.data()) : reinterpret_cast<void*>(ModelGeo.MeshIndices.data());

	CreateBuffer(Device.Get(), RenderThreadCmdList.Get(), InitData, ModelGeo.IndiceSize, ModelGeo.IndexBuffer, ModelGeo.IndexBufferUpload);

	NAME_D3D12_OBJECT(ModelGeo.IndexBuffer);
	NAME_D3D12_OBJECT(ModelGeo.IndexBufferUpload);

	ModelGeo.CreateIndexBufferView();
}

void GraphicRenderModel::CreateTextureAndSampler()
{
	DirectX::CreateDDSTextureFromFile12(Device.Get(), RenderThreadCmdList.Get(), TexFileName.c_str(), ModelGeo.TextureResource, ModelGeo.TextureResourceUpload);
	NAME_D3D12_OBJECT(ModelGeo.TextureResource);
	NAME_D3D12_OBJECT(ModelGeo.TextureResourceUpload);

	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = ModelGeo.GetShaderResourceViewDesc();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvHandle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), 1, CbvSrvUavDescriptorSize);
	Device->CreateShaderResourceView(ModelGeo.TextureResource.Get(), &SrvDesc, CbvSrvHandle);

	// Describe and create a sampler.
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
	Device->CreateSampler(&SamplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicRenderModel::FlushCommandQueue()
{
	//Create synchronization objects and wait until assets have been uploaded to the GPU.
	UINT64 FenceNum = FenceValue;
	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), FenceNum));
	FenceValue++;
	ThrowIfFailed(Fence->SetEventOnCompletion(FenceNum, FenceEvent));
}

void GraphicRenderModel::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePoint.x = x; 
	LastMousePoint.y = y;
	SetCapture(MainApplication->GetHwnd());
}

void GraphicRenderModel::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void GraphicRenderModel::OnMouseMove(WPARAM btnState, int x, int y)
{
	if((btnState & MK_LBUTTON) != 0 ) 
	{
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - LastMousePoint.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - LastMousePoint.y));
		Camera.ChangeViewMatrixByMouseEvent(dx, dy);
	}

	LastMousePoint.x = x; 
	LastMousePoint.y = y;
}

void GraphicRenderModel::Update()
{
	
}

bool GraphicRenderModel::Render()
{
	DWORD Ret = WaitForMultipleObjects(static_cast<DWORD>(HWaitedArr.size()), HWaitedArr.data(), TRUE, INFINITE);
	if( Ret - WAIT_OBJECT_0 == 0 )
	{
		switch (State)
		{
		case RENDER_INIT:// first create vertex/index/texture view, and push to GPU to excute
		{
			// Execute the initialization commands.
			RenderThreadCmdList->Close();
			ID3D12CommandList* CmdsLists[] = { RenderThreadCmdList.Get() };
			CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
			//// Wait until initialization is complete.
			FlushCommandQueue();
			State = RENDER_PRE;
			HWaitedArr.clear();
			HWaitedArr.push_back(FenceEvent);
		}
		break;
		case RENDER_PRE://
		{
			ThrowIfFailed(CommandAllocatorPre->Reset());
			ThrowIfFailed(CommandListPre->Reset(CommandAllocatorPre.Get(), PipelineState.Get()));

			ThrowIfFailed(CommandAllocatorPost->Reset());
			ThrowIfFailed(CommandListPost->Reset(CommandAllocatorPost.Get(), PipelineState.Get()));

			CommandListPre->RSSetViewports(1, &Viewport);
			CommandListPre->RSSetScissorRects(1, &ScissorRect);

			// Indicate that the back buffer will be used as a render target.
			const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			CommandListPre->ResourceBarrier(1, &Rb1);

			//clear back buffer and depth buffer
			CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RtvDescriptorSize);
			CommandListPre->ClearRenderTargetView(RtvHandle, Colors::LightSteelBlue, 0, nullptr);
			CommandListPre->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
			D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
			CommandListPre->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);
			State = RENDER_POST;

			HWaitedArr.clear();
			HWaitedArr.push_back(ThreadParam.RenderEnd);
			//ThreadParam.FrameIndex = FrameIndex;
			SetEvent(ThreadParam.RenderRun);
		}
		break;
		case RENDER_POST:
		{
			const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			CommandListPost->ResourceBarrier(1, &Rb2);

			CommandListPre->Close();
			CommandListPost->Close();

			ID3D12CommandList* ppCommandLists[] = { CommandListPre.Get(), RenderThreadCmdList.Get(), CommandListPost.Get() };
			CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
			ThrowIfFailed(SwapChain->Present(1, 0));
			FlushCommandQueue();
			State = RENDER_PRE;

			HWaitedArr.clear();
			HWaitedArr.push_back(FenceEvent);

			FrameIndex = (FrameIndex + 1) % FrameCount;
		}
		break;
		default:{}
		}
	}

	return true;
}

void GraphicRenderModel::Destroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	bDestroy = true;

	CloseHandle(ThreadParam.ThisThread);
	CloseHandle(ThreadParam.RenderBegin);
	CloseHandle(ThreadParam.RenderRun);
	CloseHandle(ThreadParam.RenderEnd);

	CommandAllocatorPre.Reset();
	CommandListPre.Reset();
	CommandAllocatorPost.Reset();
	CommandListPost.Reset();
	RenderThreadCmdAllocator.Reset();
	RenderThreadCmdList.Reset();

	Fence.Reset();
	PipelineState.Reset();
	RootSignature.Reset();
	RtvHeap.Reset();
	CbvSrvHeap.Reset();
	DsvHeap.Reset();
	SamplerHeap.Reset();
	ResetComPtrArray(&RenderTargets);
	DepthStencilBuffer.Reset();

	ConstantBuffer->Unmap(0, nullptr);
	ConstantBuffer.Reset();
	ModelGeo.Destroy();
	
	CommandQueue.Reset();
	SwapChain.Reset();
	Device.Reset();
	
	ThisRenderModel = nullptr;
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