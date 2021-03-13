

#include "stdafx.h"
#include "GraphicRender_LoadModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

#include "ApplicationMain.h"
#include "DataSource.h"

using namespace Microsoft::WRL;

GraphicRender_LoadModel::GraphicRender_LoadModel()
	: FrameIndex(0)
	, RtvDescriptorSize(0)
	, VertexBufferView(D3D12_VERTEX_BUFFER_VIEW())
	, FenceValue(0)
	, pCbvDataBegin(nullptr)
	, ObjectConstant({})
	, CbvSrvUavDescriptorSize(0)
	, DsvDescriptorSize(0)
	, IndexBufferView(D3D12_INDEX_BUFFER_VIEW())
	, IndiceSize(0)
	, IndicesCount(0)
{
	MtWorld = MathHelper::Identity4x4();
	MtView = MathHelper::Identity4x4();
	MtProj = MathHelper::Identity4x4();

	Theta = 1.5f * XM_PI;
	Phi = XM_PIDIV4;
	Radius = 5.0f;
}

GraphicRender_LoadModel::~GraphicRender_LoadModel()
{

}

void GraphicRender_LoadModel::OnInit()
{
	InitCamera();
	LoadPipline();
	LoadAssets();
}

void GraphicRender_LoadModel::InitCamera()
{
	DataSource* Ds = MainApplication->GetDataSource();
	const CameraData& CData = Ds->GetCameraData();
	Camera.Init(CData.Location, CData.Target, CData.Rotator);
}

void GraphicRender_LoadModel::LoadPipline()
{
	CreateDxgiObjects();
	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
	OnResize();
}

void GraphicRender_LoadModel::LoadAssets()
{
	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	CreateConstantBuffer();
	CreateRootSignature();
	LoadShadersAndCreatePso();
	CreateVertexBuffer();
	CreateIndexBuffer();

	// Execute the initialization commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();
}

void GraphicRender_LoadModel::CreateDxgiObjects()
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
			ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&DxgiFactory)));
		}
	}
#endif
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

	RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	CbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void GraphicRender_LoadModel::CreateCommandObjects()
{
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(Device->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	NAME_D3D12_OBJECT(CommandQueue);

	ThrowIfFailed(Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	NAME_D3D12_OBJECT(CommandAllocator);

	// Create the command list.
	ThrowIfFailed(Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&CommandList)));
	NAME_D3D12_OBJECT(CommandList);
	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(CommandList->Close());
}

void GraphicRender_LoadModel::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	SwapChain.Reset();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 SwapChainDesc = {};
	SwapChainDesc.BufferCount = FrameCount;
	SwapChainDesc.Width = WndWidth;
	SwapChainDesc.Height = WndHeight;
	SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	SwapChainDesc.SampleDesc.Count = 1;

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

void GraphicRender_LoadModel::CreateDescriptorHeaps()
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
		
		D3D12_DESCRIPTOR_HEAP_DESC CbvHeapDesc = {};
		CbvHeapDesc.NumDescriptors = 1;
		CbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		CbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		CbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(Device->CreateDescriptorHeap(&CbvHeapDesc,
			IID_PPV_ARGS(&CbvHeap)));
		NAME_D3D12_OBJECT(CbvHeap);
	}
}

void GraphicRender_LoadModel::CreateConstantBuffer()
{
	//constant buffer size must 256's multiple
	UINT ConstantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;
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
	Device->CreateConstantBufferView(&CbvDesc, CbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicRender_LoadModel::CreateRootSignature()
{
	// Create a root Signature consisting of a descriptor table with a single CBV.
	CD3DX12_ROOT_PARAMETER RootParameters[1];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[1];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0]);

	// Allow input layout and deny uneccessary access to certain pipeline stages.

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1, RootParameters, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> Signature;
	ComPtr<ID3DBlob> Error;
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
	ThrowIfFailed(Device->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
	NAME_D3D12_OBJECT(RootSignature);
}

void GraphicRender_LoadModel::LoadShadersAndCreatePso()
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
	D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &pVertexShader, &ErrorsVs);
	D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pPixelShader, &ErrorsPs);
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PsoDesc.InputLayout = { InputElementDescs, _countof(InputElementDescs) };
	PsoDesc.pRootSignature = RootSignature.Get();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShader.Get());
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShader.Get());
	PsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = 1;
	PsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(Device->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PipelineState)));
	NAME_D3D12_OBJECT(PipelineState);
}

void GraphicRender_LoadModel::CreateVertexBuffer()
{
	DataSource* Ds = MainApplication->GetDataSource();
	std::vector<Vertex_PositionColor> TriangleVertices;
	// Create the vertex buffer.
	// Define the geometry for a triangle.  read the vertices data
	Ds->GetPositionColorInput(TriangleVertices);
	const UINT VertexBufferSize = static_cast<UINT>(TriangleVertices.size() * sizeof(Vertex_PositionColor));//sizeof(TriangleVertices);

	const CD3DX12_HEAP_PROPERTIES VertexDefaultProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC VertexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertexDefaultProp,
		D3D12_HEAP_FLAG_NONE,
		&VertexDefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&VertexBuffer)));
	NAME_D3D12_OBJECT(VertexBuffer);

	//create vertex upload heap
	const CD3DX12_HEAP_PROPERTIES VertextUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC VertextUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(VertexBufferSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&VertextUploadProp,
		D3D12_HEAP_FLAG_NONE,
		&VertextUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&VertexUploadBuffer)));
	NAME_D3D12_OBJECT(VertexUploadBuffer);

	D3D12_SUBRESOURCE_DATA SubResourceData;
	SubResourceData.pData = TriangleVertices.data();
	SubResourceData.RowPitch = VertexBufferSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	const D3D12_RESOURCE_BARRIER RbVertex1 = CD3DX12_RESOURCE_BARRIER::Transition(VertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->ResourceBarrier(1, &RbVertex1);
	UpdateSubresources<1>(CommandList.Get(), VertexBuffer.Get(), VertexUploadBuffer.Get(), 0, 0, 1, &SubResourceData);
	const D3D12_RESOURCE_BARRIER RbVertex2 = CD3DX12_RESOURCE_BARRIER::Transition(VertexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	CommandList->ResourceBarrier(1, &RbVertex2);

	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = sizeof(Vertex_PositionColor);
	VertexBufferView.SizeInBytes = VertexBufferSize;
}

void GraphicRender_LoadModel::CreateIndexBuffer()
{
	//create index buffer 
	DataSource* Ds = MainApplication->GetDataSource();
	bool bUseHalfInt32 = Ds->IsIndicesValueHalfInt32();
	if (!bUseHalfInt32)
	{
		IndicesCount = static_cast<UINT>(Ds->GetIndexDataInput().size());
		IndiceSize = IndicesCount * sizeof(UINT);
	}
	else
	{
		IndicesCount = static_cast<UINT>(Ds->GetIndexDataValueHalfInput().size());
		IndiceSize = IndicesCount * sizeof(UINT16);
	}

	const CD3DX12_HEAP_PROPERTIES IndexDefaultPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC IndexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(IndiceSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&IndexDefaultPro,
		D3D12_HEAP_FLAG_NONE,
		&IndexDefaultDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&IndexBuffer)));
	NAME_D3D12_OBJECT(IndexBuffer);

	const CD3DX12_HEAP_PROPERTIES IndexUploadPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC IndexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(IndiceSize);
	ThrowIfFailed(Device->CreateCommittedResource(
		&IndexUploadPro,
		D3D12_HEAP_FLAG_NONE,
		&IndexUploadDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&IndexBufferUpload)));
	NAME_D3D12_OBJECT(IndexBufferUpload);

	D3D12_SUBRESOURCE_DATA SubResourceData;
	if (bUseHalfInt32)
		SubResourceData.pData = Ds->GetIndexDataValueHalfInput().data();
	else
		SubResourceData.pData = Ds->GetIndexDataInput().data();

	SubResourceData.RowPitch = IndiceSize;
	SubResourceData.SlicePitch = SubResourceData.RowPitch;

	const D3D12_RESOURCE_BARRIER RbIndex1 = CD3DX12_RESOURCE_BARRIER::Transition(IndexBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST);
	CommandList->ResourceBarrier(1, &RbIndex1);
	UpdateSubresources<1>(CommandList.Get(), IndexBuffer.Get(), IndexBufferUpload.Get(), 0, 0, 1, &SubResourceData);
	const D3D12_RESOURCE_BARRIER RbIndex2 = CD3DX12_RESOURCE_BARRIER::Transition(IndexBuffer.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_GENERIC_READ);
	CommandList->ResourceBarrier(1, &RbIndex2);

	IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
	IndexBufferView.Format = bUseHalfInt32 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	IndexBufferView.SizeInBytes = IndiceSize;

}



void GraphicRender_LoadModel::FlushCommandQueue()
{
	//Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		FenceValue++;
		ThrowIfFailed(CommandQueue->Signal(Fence.Get(), FenceValue));
		if (Fence->GetCompletedValue() < FenceValue)
		{
			// Create an event handle to use for frame synchronization.
			HANDLE FenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
			ThrowIfFailed(Fence->SetEventOnCompletion(FenceValue, FenceEvent));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(FenceEvent, INFINITE);
			CloseHandle(FenceEvent);
		}
	}
}

void GraphicRender_LoadModel::OnResize()
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	DataSource* Ds = MainApplication->GetDataSource();
	const CameraData& CData = Ds->GetCameraData();
	AspectRatio = static_cast<float>(WndWidth) / WndHeight;
	Camera.InitFovAndAspect(XMConvertToRadians(CData.Fov), AspectRatio);
	XMMATRIX P = Camera.GetProjectionMatrix();
	XMStoreFloat4x4(&MtProj, P);

	FlushCommandQueue();

	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
	for (int i = 0; i < FrameCount; ++i)
		RenderTargets[i].Reset();
	DepthStencilBuffer.Reset();

	ThrowIfFailed(SwapChain->ResizeBuffers(
		FrameCount,
		WndWidth, WndHeight,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	FrameIndex = 0;

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(SwapChain->GetBuffer(n, IID_PPV_ARGS(&RenderTargets[n])));
			Device->CreateRenderTargetView(RenderTargets[n].Get(), nullptr, RtvHandle);
			RtvHandle.Offset(1, RtvDescriptorSize);
		}
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC DepthStencilDesc = {};
	DepthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	DepthStencilDesc.Alignment = 0;
	DepthStencilDesc.Width = WndWidth;
	DepthStencilDesc.Height = WndHeight;
	DepthStencilDesc.DepthOrArraySize = 1;
	DepthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	DepthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	DepthStencilDesc.SampleDesc.Count = 1;
	DepthStencilDesc.SampleDesc.Quality = 0;
	DepthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	DepthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE OptClear;
	OptClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	OptClear.DepthStencil.Depth = 1.0f;
	OptClear.DepthStencil.Stencil = 0;

	const CD3DX12_HEAP_PROPERTIES Property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(Device->CreateCommittedResource(
		&Property,
		D3D12_HEAP_FLAG_NONE,
		&DepthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&OptClear,
		IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC DsvDesc;
	DsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	DsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	DsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DsvDesc.Texture2D.MipSlice = 0;
	Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &DsvDesc, DsvHeap->GetCPUDescriptorHandleForHeapStart());

	// Transition the resource from its initial state to be used as a depth buffer.
	const CD3DX12_RESOURCE_BARRIER Rb = CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	CommandList->ResourceBarrier(
		1,
		&Rb);

	// Execute the resize commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();
}

void GraphicRender_LoadModel::Update()
{
	//calculate model matrix : scale * rotation * translation
	//in ue4: ue4x = Forward ue4y = Right ue4z = Up
	//in direct x: use left hand coordinate, x = Right, y = Up, z = Forward
	//we have the conversion: x = ue4y, y = ue4z, z = ue4x, Yaw Pitch Roll stay the same

	DataSource* Ds = MainApplication->GetDataSource();
	const CameraData& CData = Ds->GetCameraData();
	XMFLOAT3 LocationTarget = MathHelper::GetUe4ConvertLocation(CData.Target);
	//determine the watch target matrix, the M view, make no scale no rotation , so we dont need to multiply scale and rotation
	XMStoreFloat4x4(&MtWorld, XMMatrixTranslation(LocationTarget.x, LocationTarget.y, LocationTarget.z));
	XMMATRIX World = XMLoadFloat4x4(&MtWorld);

	//determine the projection matrix
	XMMATRIX Proj = XMLoadFloat4x4(&MtProj);
	XMMATRIX WorldViewProj = World * Camera.GetViewMarix() * Proj;

	 //Update the constant buffer with the latest WorldViewProj matrix.
	XMStoreFloat4x4(&ObjectConstant.WorldViewProj, XMMatrixTranspose(WorldViewProj));
	memcpy(pCbvDataBegin, &ObjectConstant, sizeof(ObjectConstant));
}

bool GraphicRender_LoadModel::Render()
{
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ThrowIfFailed(CommandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), PipelineState.Get()));

	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &Rb1);

	//clear back buffer and depth buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RtvDescriptorSize);
	CommandList->ClearRenderTargetView(RtvHandle, Colors::LightSteelBlue, 0, nullptr);
	CommandList->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);

	ID3D12DescriptorHeap* ppHeaps[] = { CbvHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// Set necessary state.
	CommandList->SetGraphicsRootSignature(RootSignature.Get());

	CommandList->IASetVertexBuffers(0, 1, &VertexBufferView);
	CommandList->IASetIndexBuffer(&IndexBufferView);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	CommandList->SetGraphicsRootDescriptorTable(0, CbvHeap->GetGPUDescriptorHandleForHeapStart());
	
	CommandList->DrawIndexedInstanced(IndicesCount, 1, 0, 0, 0);

	// Indicate that the back buffer will now be used to present.
	const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(1, &Rb2);

	ThrowIfFailed(CommandList->Close());

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(SwapChain->Present(0, 0));
	FrameIndex = (FrameIndex + 1) % FrameCount;

	FlushCommandQueue();
	return true;
}

void GraphicRender_LoadModel::Destroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.

	Fence.Reset();
	PipelineState.Reset();
	RootSignature.Reset();
	CommandList.Reset();
	CommandAllocator.Reset();
	RtvHeap.Reset();
	CbvHeap.Reset();
	DsvHeap.Reset();
	ResetComPtrArray(&RenderTargets);
	DepthStencilBuffer.Reset();
	VertexBuffer.Reset();
	VertexUploadBuffer.Reset();
	IndexBuffer.Reset();
	IndexBufferUpload.Reset();
	ConstantBuffer.Reset();
	CommandQueue.Reset();
	SwapChain.Reset();
	Device.Reset();

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