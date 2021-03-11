

#include "stdafx.h"
#include "GraphicRender_LoadModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>

#include "ApplicationMain.h"
#include "DataSource.h"

using namespace Microsoft::WRL;

GraphicRender_LoadModel::GraphicRender_LoadModel()
	: m_frameIndex(0)
	, m_rtvDescriptorSize(0)
	, m_vertexBufferView(D3D12_VERTEX_BUFFER_VIEW())
	, m_fenceEvent(nullptr)
	, m_fenceValue(0)
	, m_pCbvDataBegin(nullptr)
	, m_objectConstant({})
	, m_cbvSrvUavDescriptorSize(0)
	, m_dsvDescriptorSize(0)
	, m_indexBufferView(D3D12_INDEX_BUFFER_VIEW())
	, m_indiceSize(0)
	, m_indicesCount(0)
{

}

GraphicRender_LoadModel::~GraphicRender_LoadModel()
{

}

void GraphicRender_LoadModel::OnInit()
{
	m_viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height));
	m_scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height));

	//m_camera.Init({ 8, 8, 30 });
	LoadPipeline();
	LoadAssets();
}

void GraphicRender_LoadModel::CreateCommandObjects()
{
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
	NAME_D3D12_OBJECT(m_commandQueue);

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	NAME_D3D12_OBJECT(m_commandAllocator);

	// Create the command list.
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
	NAME_D3D12_OBJECT(m_commandList);
	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	ThrowIfFailed(m_commandList->Close());
}

void GraphicRender_LoadModel::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	m_swapChain.Reset();

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count =  1;
	sd.SampleDesc.Quality =  0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = FrameCount;
	sd.OutputWindow = m_application->GetHwnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
		m_commandQueue.Get(),       
		&sd,
		&m_swapChain));

}

void GraphicRender_LoadModel::CreateDescriptorHeaps()
{
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
		NAME_D3D12_OBJECT(m_rtvHeap);

		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		dsvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateDescriptorHeap(
			&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
		NAME_D3D12_OBJECT(m_dsvHeap);
		
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
		cbvHeapDesc.NumDescriptors = 1;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc,
			IID_PPV_ARGS(&m_cbvHeap)));
		NAME_D3D12_OBJECT(m_cbvHeap);
	}
}

void GraphicRender_LoadModel::FlushCommandQueue()
{
	//Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		m_fenceValue++;
		ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
		if (m_fence->GetCompletedValue() < m_fenceValue)
		{
			// Create an event handle to use for frame synchronization.
			HANDLE fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
			ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, fenceEvent));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(fenceEvent, INFINITE);
			CloseHandle(fenceEvent);
		}
	}
}

void GraphicRender_LoadModel::OnResize()
{
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	float AspectRatio = static_cast<float>(m_width) / m_height;
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	FlushCommandQueue();

	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
	for (int i = 0; i < FrameCount; ++i)
		m_renderTargets[i].Reset();
	m_depthStencilBuffer.Reset();

	ThrowIfFailed(m_swapChain->ResizeBuffers(
		FrameCount,
		m_width, m_height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_frameIndex = 0;

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Create a RTV for each frame.
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc = {};
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_width;
	depthStencilDesc.Height = m_height;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count =  1;
	depthStencilDesc.SampleDesc.Quality =  0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	const CD3DX12_HEAP_PROPERTIES Property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	ThrowIfFailed(m_device->CreateCommittedResource(
		&Property,
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	// Transition the resource from its initial state to be used as a depth buffer.
	const CD3DX12_RESOURCE_BARRIER rb = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
	m_commandList->ResourceBarrier(
		1, 
		&rb);

	// Execute the resize commands.
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();
}

void GraphicRender_LoadModel::LoadPipeline()
{

#if defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
			ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));
		}
	}
#endif
	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_device));

	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)));
	}
	NAME_D3D12_OBJECT(m_device);

	ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
	NAME_D3D12_OBJECT(m_fence);

	m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateCommandObjects();
	CreateSwapChain();
	CreateDescriptorHeaps();
	OnResize();
}

void GraphicRender_LoadModel::LoadAssets()
{
	// Reset the command list to prep for initialization commands.
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	//create constant buffer
	{
		//constant buffer的大小必须是 256的倍数
		UINT constantBufferSize = (sizeof(ObjectConstants) + 255) & ~255;
		const CD3DX12_HEAP_PROPERTIES ConstantProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC ConstantDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

		ThrowIfFailed(m_device->CreateCommittedResource(
			&ConstantProp,
			D3D12_HEAP_FLAG_NONE,
			&ConstantDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_constantBuffer)));

		CD3DX12_RANGE readRange(0, 0);
		ThrowIfFailed(m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pCbvDataBegin)));
		memcpy(m_pCbvDataBegin, &m_objectConstant, sizeof(m_objectConstant));
		NAME_D3D12_OBJECT(m_constantBuffer);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = constantBufferSize;
		m_device->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
	}

	// Create a root signature consisting of a descriptor table with a single CBV.
	{
		CD3DX12_ROOT_PARAMETER rootParameters[1];
		CD3DX12_DESCRIPTOR_RANGE ranges[1];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);

		// Allow input layout and deny uneccessary access to certain pipeline stages.

		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(1, rootParameters, 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
		ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
		NAME_D3D12_OBJECT(m_rootSignature);
	}


	// Create the pipeline state, which includes compiling and loading shaders.
	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif
	ComPtr<ID3DBlob> errors1, errors2;
	D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &errors1);
	D3DCompileFromFile(GetAssetFullPath(L"shaders.hlsl").c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &errors2);
	if (errors1 != nullptr)
	{
		OutputDebugStringA((char*)errors1->GetBufferPointer());
	}
	if (errors2 != nullptr)
	{
		OutputDebugStringA((char*)errors2->GetBufferPointer());
	}
	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	DataSource* ds = m_application->GetDataSource();
	std::vector<Vertex_PositionColor> triangleVertices;
	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.  read the vertices data
		ds->GetPositionColorInput(triangleVertices);
		const UINT vertexBufferSize = static_cast<UINT>(triangleVertices.size() * sizeof(Vertex_PositionColor));//sizeof(triangleVertices);
		
		const CD3DX12_HEAP_PROPERTIES VertexDefaultProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC VertexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		ThrowIfFailed(m_device->CreateCommittedResource(
			&VertexDefaultProp,
			D3D12_HEAP_FLAG_NONE,
			&VertexDefaultDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_vertexBuffer)));
		NAME_D3D12_OBJECT(m_vertexBuffer);

		//create vertex upload heap
		const CD3DX12_HEAP_PROPERTIES VertextUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC VertextUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		ThrowIfFailed(m_device->CreateCommittedResource(
			&VertextUploadProp,
			D3D12_HEAP_FLAG_NONE,
			&VertextUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_vertexUploadBuffer)));
		NAME_D3D12_OBJECT(m_vertexUploadBuffer);

		D3D12_SUBRESOURCE_DATA subResourceData;
		subResourceData.pData = triangleVertices.data();
		subResourceData.RowPitch = vertexBufferSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		const D3D12_RESOURCE_BARRIER rbVertex1 = CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);
		m_commandList->ResourceBarrier(1, &rbVertex1);
		UpdateSubresources<1>(m_commandList.Get(), m_vertexBuffer.Get(), m_vertexUploadBuffer.Get(), 0, 0, 1, &subResourceData);
		const D3D12_RESOURCE_BARRIER rbVertex2 = CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		m_commandList->ResourceBarrier(1, &rbVertex2);

		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.StrideInBytes = sizeof(Vertex_PositionColor);
		m_vertexBufferView.SizeInBytes = vertexBufferSize;

	}

	//create index buffer 
	{
		const std::vector<UINT32>& indices = ds->GetIndexDataInput();
		m_indiceSize = static_cast<UINT>(indices.size() * sizeof(UINT));
		m_indicesCount = static_cast<UINT>(indices.size());

		const CD3DX12_HEAP_PROPERTIES indexDefaultPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC indexDefaultDesc = CD3DX12_RESOURCE_DESC::Buffer(m_indiceSize);
		ThrowIfFailed(m_device->CreateCommittedResource(
			&indexDefaultPro,
			D3D12_HEAP_FLAG_NONE,
			&indexDefaultDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&m_indexBuffer)));
		NAME_D3D12_OBJECT(m_indexBuffer);

		const CD3DX12_HEAP_PROPERTIES indexUploadPro = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC indexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(m_indiceSize);
		ThrowIfFailed(m_device->CreateCommittedResource(
			&indexUploadPro,
			D3D12_HEAP_FLAG_NONE,
			&indexUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_indexBufferUpload)));
		NAME_D3D12_OBJECT(m_indexBufferUpload);

		D3D12_SUBRESOURCE_DATA subResourceData;
		subResourceData.pData = indices.data();
		subResourceData.RowPitch = m_indiceSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		const D3D12_RESOURCE_BARRIER rbIndex1 = CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST);
		m_commandList->ResourceBarrier(1, &rbIndex1);
		UpdateSubresources<1>(m_commandList.Get(), m_indexBuffer.Get(), m_indexBufferUpload.Get(), 0, 0, 1, &subResourceData);
		const D3D12_RESOURCE_BARRIER rbIndex2 = CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ);
		m_commandList->ResourceBarrier(1, &rbIndex2);

		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = m_indiceSize;

	}

	//create pipline state object
	{
		// Describe and create the graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;	////不使用4XMSAA
		psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
		NAME_D3D12_OBJECT(m_pipelineState);
	}

	// Execute the initialization commands.
	ThrowIfFailed(m_commandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete.
	FlushCommandQueue();

}

void GraphicRender_LoadModel::Update()
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y * 100.f, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);

	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX worldViewProj = world * view * proj;

	// Update the constant buffer with the latest worldViewProj matrix.
	XMStoreFloat4x4(&m_objectConstant.worldViewProj, XMMatrixTranspose(worldViewProj));
	memcpy(m_pCbvDataBegin, &m_objectConstant, sizeof(m_objectConstant));
}

bool GraphicRender_LoadModel::Render()
{
	// Command list allocators can only be reset when the associated 
  // command lists have finished execution on the GPU; apps should use 
  // fences to determine GPU execution progress.
	ThrowIfFailed(m_commandAllocator->Reset());

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Indicate that the back buffer will be used as a render target.
	const D3D12_RESOURCE_BARRIER rb1 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &rb1);

	//clear back buffer and depth buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
	m_commandList->ClearRenderTargetView(rtvHandle, Colors::LightSteelBlue, 0, nullptr);
	m_commandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	m_commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap.Get() };
	m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// Set necessary state.
	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->IASetIndexBuffer(&m_indexBufferView);
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	m_commandList->SetGraphicsRootDescriptorTable(0, m_cbvHeap->GetGPUDescriptorHandleForHeapStart());
	
	m_commandList->DrawIndexedInstanced(m_indicesCount, 1, 0, 0, 0);

	// Indicate that the back buffer will now be used to present.
	const D3D12_RESOURCE_BARRIER rb2 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &rb2);

	ThrowIfFailed(m_commandList->Close());

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	ThrowIfFailed(m_swapChain->Present(0, 0));
	m_frameIndex = (m_frameIndex + 1) % FrameCount;

	FlushCommandQueue();
	return true;
}

void GraphicRender_LoadModel::Destroy()
{
	// Ensure that the GPU is no longer referencing resources that are about to be
	// cleaned up by the destructor.
	CloseHandle(m_fenceEvent);

	m_fence.Reset();
	m_pipelineState.Reset();
	m_rootSignature.Reset();
	m_commandList.Reset();
	m_commandAllocator.Reset();
	m_rtvHeap.Reset();
	m_cbvHeap.Reset();
	m_dsvHeap.Reset();
	ResetComPtrArray(&m_renderTargets);
	m_depthStencilBuffer.Reset();
	m_vertexBuffer.Reset();
	m_vertexUploadBuffer.Reset();
	m_indexBuffer.Reset();
	m_indexBufferUpload.Reset();
	m_constantBuffer.Reset();
	m_commandQueue.Reset();
	m_swapChain.Reset();
	m_device.Reset();

#if defined(_DEBUG)
	{
		ComPtr<IDXGIDebug1> dxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
		{
			dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
}