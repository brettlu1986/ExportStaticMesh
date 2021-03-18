

#include "stdafx.h"
#include "GraphicRender_LoadModel.h"
#include <dxgidebug.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <iostream>
#include "ApplicationMain.h"
#include "d3dx12.h"

using namespace Microsoft::WRL;

GraphicRender_LoadModel::GraphicRender_LoadModel()
	: FrameIndex(0)
	, RtvDescriptorSize(0)
	, FenceValue(0)
	, CbvSrvUavDescriptorSize(0)
	, DsvDescriptorSize(0)
	
{
	MtProj = MathHelper::Identity4x4();
	LastMousePoint = {0 , 0};
}

GraphicRender_LoadModel::~GraphicRender_LoadModel()
{

}

void GraphicRender_LoadModel::OnInit()
{
	InitCamera();
	InitModel();
	LoadPipline();
	LoadAssets();
}

void GraphicRender_LoadModel::InitCamera()
{
	Camera.Init();
}

void GraphicRender_LoadModel::InitModel()
{
	ModelGeo.Init();
	ModelGeo.SetModelLocation(Camera.GetViewTargetLocation());
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

	CreateRootSignature();
	CreateConstantBufferView();
	LoadShadersAndCreatePso();
	CreateVertexBufferView();
	CreateIndexBufferView();
	CreateTextureAndSampler();

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

void GraphicRender_LoadModel::CreateRootSignature()
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

void GraphicRender_LoadModel::CreateConstantBufferView()
{
	ModelGeo.CreateConstantBuffer(Device.Get());
	D3D12_CONSTANT_BUFFER_VIEW_DESC ConstantBufferDesc = ModelGeo.GetConstantBufferDesc();
	Device->CreateConstantBufferView(&ConstantBufferDesc, CbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
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
}

void GraphicRender_LoadModel::CreateVertexBufferView()
{
	ModelGeo.CreateVertexBufferView(Device.Get(), CommandList.Get());
}

void GraphicRender_LoadModel::CreateIndexBufferView()
{
	ModelGeo.CreateIndexBufferView(Device.Get(), CommandList.Get());
}

void GraphicRender_LoadModel::CreateTextureAndSampler()
{
	ModelGeo.CreateDDSTextureFomFile(Device.Get(), CommandList.Get());
	D3D12_SHADER_RESOURCE_VIEW_DESC SrvDesc = ModelGeo.GetShaderResourceViewDesc();
	CD3DX12_CPU_DESCRIPTOR_HANDLE CbvSrvHandle(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), 1, CbvSrvUavDescriptorSize);
	Device->CreateShaderResourceView(ModelGeo.GetTextureResource(), &SrvDesc, CbvSrvHandle);

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
	Camera.OnResize(static_cast<float>(WndWidth), static_cast<float>(WndHeight));
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

	// Execute the resize commands.
	ThrowIfFailed(CommandList->Close());
	ID3D12CommandList* CmdsLists[] = { CommandList.Get() };
	CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();
}

void GraphicRender_LoadModel::OnMouseDown(WPARAM btnState, int x, int y)
{
	LastMousePoint.x = x; 
	LastMousePoint.y = y;
	SetCapture(MainApplication->GetHwnd());
}

void GraphicRender_LoadModel::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void GraphicRender_LoadModel::OnMouseMove(WPARAM btnState, int x, int y)
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

void GraphicRender_LoadModel::Update()
{
	ModelGeo.UpdateConstantBuffers(Camera.GetViewMarix(), XMLoadFloat4x4(&MtProj));
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
	CommandList->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);

	ID3D12DescriptorHeap* ppHeaps[] = { CbvSrvHeap.Get(), SamplerHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// Set necessary state.
	CommandList->SetGraphicsRootSignature(RootSignature.Get());

	CommandList->IASetVertexBuffers(0, 1, &ModelGeo.GetVertexBufferView());
	CommandList->IASetIndexBuffer(&ModelGeo.GetIndexBufferView());
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	CommandList->SetGraphicsRootDescriptorTable(0, CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE tex(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	tex.Offset(1, CbvSrvUavDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(1, tex);
	CommandList->SetGraphicsRootDescriptorTable(2, SamplerHeap->GetGPUDescriptorHandleForHeapStart());
	
	CommandList->DrawIndexedInstanced(ModelGeo.GetIndicesCount(), 1, 0, 0, 0);

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
	CbvSrvHeap.Reset();
	DsvHeap.Reset();
	SamplerHeap.Reset();
	ResetComPtrArray(&RenderTargets);
	DepthStencilBuffer.Reset();
	ModelGeo.Destroy();
	
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