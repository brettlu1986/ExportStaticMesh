
#include "stdafx.h"

#include <dxgidebug.h>
#include <d3dcompiler.h>
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FDefine.h"
#include "LAssetDataLoader.h"
#include "LEngine.h"
#include "d3dx12.h"
#include "LDeviceWindows.h"
#include "FD3D12ResourceViewCreater.h"

using namespace Microsoft::WRL;
using namespace DirectX;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

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

		FrameIndex = 0;
		ThrowIfFailed(D3DDevice.Get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
		NAME_D3D12_OBJECT(CommandAllocator);

		ThrowIfFailed(D3DDevice.Get()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
			nullptr, IID_PPV_ARGS(&CommandList)));
		NAME_D3D12_OBJECT(CommandList);
		ThrowIfFailed(CommandList->Close());

		ThrowIfFailed(D3DDevice.Get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&GpuFence)));
		NAME_D3D12_OBJECT(GpuFence);
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		memset(&FenceValues, 0, sizeof(FenceValues));

	}
}

void FD3D12DynamicRHI::CreateDescriptorHeaps()
{
	//dsv + 1 DSV for shader map
	CreateDescripterHeap(2, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0, IID_PPV_ARGS(&DsvHeap));
	NAME_D3D12_OBJECT(DsvHeap);

	//normal texture sampler 
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


	CD3DX12_STATIC_SAMPLER_DESC shadow(
		1, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);
	StaticSamplers.push_back(shadow);
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
	depthOptimizedClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	CD3DX12_HEAP_PROPERTIES ProDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC ResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R24G8_TYPELESS, WndWidth, WndHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
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
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
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

	for (auto Pso : PiplelineStateObjCache)
	{
		Pso.second->Destroy();
	}
	PiplelineStateObjCache.clear();

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

	if(ShaderMap)
	{
		ShaderMap->Destroy();
		ShaderMap = nullptr;
	}

	CommandAllocator.Reset();
	CommandList.Reset();
	CommandQueue.Reset();
	D3DDevice.Reset();

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

FShader* FD3D12DynamicRHI::RHICreateShader(LPCWSTR  ShaderFile)
{
	UINT8* ShaderData = nullptr;
	UINT ShaderLen = 0;
	ThrowIfFailed(ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(ShaderFile).c_str(), &ShaderData, &ShaderLen));
	return new FShader(ShaderData, ShaderLen);
}

void FD3D12DynamicRHI::BeginRenderScene()
{
	ThrowIfFailed(CommandAllocator->Reset());
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));

	ID3D12DescriptorHeap* ppHeaps[] = { CbvSrvHeap.Get(), SamplerHeap.Get() };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	CommandList->ResourceBarrier(1, &Rb1);
}

void FD3D12DynamicRHI::DrawSceneToShadowMap(FScene* RenderScene)
{
	CommandList->SetGraphicsRootSignature(PiplelineStateObjCache["ShaderPass"]->RootSignature.Get());
	CommandList->SetPipelineState(PiplelineStateObjCache["ShaderPass"]->PipelineState.Get());

	D3D12_VIEWPORT ViewPort = ShaderMap->Viewport();
	CommandList->RSSetViewports(1, &ViewPort);
	D3D12_RECT Rect = ShaderMap->ScissorRect();
	CommandList->RSSetScissorRects(1, &Rect);

	const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(ShaderMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	CommandList->ResourceBarrier(1, &Rb1);

	CommandList->ClearDepthStencilView(ShaderMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	CD3DX12_CPU_DESCRIPTOR_HANDLE DsvHandle = ShaderMap->Dsv();
	CommandList->OMSetRenderTargets(0, nullptr, false, &DsvHandle);

	CD3DX12_GPU_DESCRIPTOR_HANDLE NulSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), 
		CurrentCbvSrvDesc.ShaderMapDesc.HeapOffsetStart + 1, CbvSrvDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(4, NulSrv);
	const std::vector<FMesh*> Meshes = RenderScene->GetDrawMeshes();
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		DrawMesh(Meshes[i], nullptr);
	}


	// Change back to GENERIC_READ so we can read the texture in a shader.
	const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(ShaderMap->Resource(),
			D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);

	//const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(ShaderMap->Resource(),
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	CommandList->ResourceBarrier(1, &Rb2);
}

void FD3D12DynamicRHI::RenderSceneObjects(FScene* Scene)
{	
	CommandList->RSSetViewports(1, &(ViewPort));
	CommandList->RSSetScissorRects(1, &(ScissorRect));

	CD3DX12_CPU_DESCRIPTOR_HANDLE RtvHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart(), FrameIndex, RtvDescriptorSize);
	float Color[4] = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };
	CommandList->ClearRenderTargetView(RtvHandle, Color, 0, nullptr);
	CommandList->ClearDepthStencilView(DsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle = DsvHeap->GetCPUDescriptorHandleForHeapStart();
	CommandList->OMSetRenderTargets(1, &RtvHandle, true, &DsvHandle);

	const std::vector<FMesh*> Meshes = Scene->GetDrawMeshes();
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		DrawMesh(Meshes[i], PiplelineStateObjCache[Meshes[i]->GetPsoKey()]);
	}
	
}

void FD3D12DynamicRHI::DrawMesh(FMesh* Mesh, FD3DGraphicPipline* Pso)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(Mesh->GetIndexBuffer());
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(Mesh->GetVertexBuffer());

	CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	if(Pso)
	{
		CommandList->SetGraphicsRootSignature(Pso->RootSignature.Get());
		CommandList->SetPipelineState(Pso->PipelineState.Get());
	}
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.CbConstant.HeapOffsetStart,
		CbvSrvDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(2, cbvSrvHandle);
	
	CD3DX12_GPU_DESCRIPTOR_HANDLE ShadowMapHandle(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.ShaderMapDesc.HeapOffsetStart,
		CbvSrvDescriptorSize);
	CommandList->SetGraphicsRootDescriptorTable(4, ShadowMapHandle);

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
		CommandList->SetGraphicsRootDescriptorTable(5, SamplerHeap->GetGPUDescriptorHandleForHeapStart());
	}

	CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
}

void FD3D12DynamicRHI::EndRenderScene()
{
	std::vector<ID3D12CommandList*> CmdLists;
	const D3D12_RESOURCE_BARRIER Rb2 = CD3DX12_RESOURCE_BARRIER::Transition(RenderTargets[FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	CommandList->ResourceBarrier(1, &Rb2);
	CommandList->Close();
	CmdLists.push_back(CommandList.Get());
	CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
	SwapChain->Present(1, 0);
	WaitForPreviousFrame();
	FrameIndex = (FrameIndex + 1) % FRAME_COUNT;
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
	const UINT64 Value = FenceValues;
	ThrowIfFailed(CommandQueue->Signal(GpuFence.Get(), Value));
	FenceValues++;
	if (GpuFence->GetCompletedValue() < Value)
	{
		ThrowIfFailed(GpuFence->SetEventOnCompletion(Value, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}
}

void FD3D12DynamicRHI::BeginCreateSceneResource()
{
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void FD3D12DynamicRHI::EndCreateSceneResource()
{
	std::vector<ID3D12CommandList*> CmdLists;
	CommandList->Close();
	CmdLists.push_back(CommandList.Get());
	CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
	FenceValues = 1;
	WaitForPreviousFrame();
	
}

void FD3D12DynamicRHI::CreateSceneResources(FScene* Scene) 
{	
	if(Scene != nullptr)
	{
		UINT ObjectNum = Scene->GetMeshCount();
		UINT PassConNum = 1;
		UINT TextureMeshNum = Scene->GetMeshWithTextureNum();
		UINT TextureShaderMapNum = 1;

		UINT MtBufferSingleSize = CalcConstantBufferByteSize(sizeof(FObjectConstants));
		UINT MtBufferTotalSize = MtBufferSingleSize * ObjectNum;

		UINT MatBufferSingleSize = CalcConstantBufferByteSize(sizeof(FMaterialConstants));
		UINT MatBufferTotalSize = MatBufferSingleSize * ObjectNum;

		UINT PassConstantSize = CalcConstantBufferByteSize(sizeof(FPassConstants));
		PassConstantSize = PassConstantSize * PassConNum;

		// material buffer count + matrix buffer count + texture count + constant buffer(1)
		CurrentCbvSrvDesc.NeedDesciptorCount = ObjectNum * 2 + TextureMeshNum + PassConNum + TextureShaderMapNum;
		CurrentCbvSrvDesc.CbMatrix = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX, 0, MtBufferTotalSize, ObjectNum };
		CurrentCbvSrvDesc.CbMaterial = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL, ObjectNum, MatBufferTotalSize , ObjectNum };
		CurrentCbvSrvDesc.CbConstant = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_PASSCONSTANT, ObjectNum * 2, PassConstantSize, PassConNum };
		CurrentCbvSrvDesc.SrvDesc = { ObjectNum * 2 + PassConNum,  TextureMeshNum };
		CurrentCbvSrvDesc.ShaderMapDesc = { ObjectNum * 2 + PassConNum + TextureMeshNum };
		CreateD3DResources();

		FShader* Vs = RHICreateShader(L"ShaderTexVs.cso");
		FShader* Ps = RHICreateShader(L"ShaderTexPs.cso");
		FRHIPiplineStateInitializer PsoInitializer = {
		   "PsoUseTexture",
		   StandardInputElementDescs,
		   _countof(StandardInputElementDescs),
		   Vs->GetShaderByteCode(),
		   Vs->GetDataLength(),
		   Ps->GetShaderByteCode(),
		   Ps->GetDataLength(),
		   1,
		   FRtvFormat::FORMAT_R8G8B8A8_UNORM,
		   FRHIRasterizerState()
		};
		CreatePipelineStateObject(PsoInitializer);

		FShader* VsNoTex = RHICreateShader(L"ShaderVs.cso");
		FShader* PsNoTex = RHICreateShader(L"ShaderPs.cso");
		FRHIPiplineStateInitializer PsoInitializerNoTex = {
		   "PsoNoTexture",
		   StandardInputElementDescs,
		   _countof(StandardInputElementDescs),
		   VsNoTex->GetShaderByteCode(),
		   VsNoTex->GetDataLength(),
		   PsNoTex->GetShaderByteCode(),
		   PsNoTex->GetDataLength(),
		   1,
		   FRtvFormat::FORMAT_R8G8B8A8_UNORM,
		   FRHIRasterizerState()
		};
		CreatePipelineStateObject(PsoInitializerNoTex);

		FShader* ShadowPassVs = RHICreateShader(L"SampleDepthShaderVs.cso");
		FShader* ShadowPassPs = RHICreateShader(L"SampleDepthShaderPs.cso");
		FRHIRasterizerState State;
		State.DepthBias = 25000;
		State.DepthBiasClamp = 0.f;
		State.SlopeScaledDepthBias = 0.1f;
		FRHIPiplineStateInitializer PsoInitializerShadowPass = {
		   "ShaderPass",
		   StandardInputElementDescs,
		   _countof(StandardInputElementDescs),
		   ShadowPassVs->GetShaderByteCode(),
		   ShadowPassVs->GetDataLength(),
		   ShadowPassPs->GetShaderByteCode(),
		   ShadowPassPs->GetDataLength(),
		   0,
		   FRtvFormat::FORMAT_UNKNOWN,
		  State
		};
		CreatePipelineStateObject(PsoInitializerShadowPass);

		delete Vs;
		delete Ps;
		delete VsNoTex;
		delete PsNoTex;
		delete ShadowPassVs;
		delete ShadowPassPs;

		Scene->InitSceneRenderResource();
	}
}

void FD3D12DynamicRHI::UpdateSceneResources(FScene* RenderScene)
{
	if(RenderScene != nullptr)
	{
		if(RenderScene->IsConstantDirty())
		{
			UpdateSceneMtConstants(RenderScene);
			UpdateSceneMatConstants(RenderScene);
			RenderScene->DecreaseDirtyCount();
		}
		UpdateScenePassConstants(RenderScene);
	}
}


void FD3D12DynamicRHI::UpdateSceneMtConstants(FScene* RenderScene)
{
	const std::vector<FMesh*>& Meshes = RenderScene->GetDrawMeshes();
	FBufferObject* ObjConsBuffer = new FBufferObject();
	ObjConsBuffer->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX;
	ObjConsBuffer->DataSize = CurrentCbvSrvDesc.CbMatrix.BufferSize;

	ObjConsBuffer->BufferData = new int8_t[CurrentCbvSrvDesc.CbMatrix.BufferSize];
	memset(ObjConsBuffer->BufferData, 0, CurrentCbvSrvDesc.CbMatrix.BufferSize);
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		FObjectConstants ObjConstants;
		XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(Meshes[i]->GetModelMatrix()));
		XMFLOAT4X4 TexMat = Meshes[i]->GetTextureTransform();
		XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
		memcpy(ObjConsBuffer->BufferData + i * CurrentCbvSrvDesc.CbMatrix.BufferSize / CurrentCbvSrvDesc.CbMatrix.BufferViewCount, &ObjConstants, sizeof(ObjConstants));
	}

	FD3DConstantBuffer* Cb = ConstantBuffers[ObjConsBuffer->Type];
	if (Cb)
	{
		Cb->UpdateConstantBufferInfo(ObjConsBuffer->BufferData, ObjConsBuffer->DataSize);
	}
	delete ObjConsBuffer;
}

void FD3D12DynamicRHI::UpdateSceneMatConstants(FScene* RenderScene)
{
	const std::vector<FMesh*>& Meshes = RenderScene->GetDrawMeshes();
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL;
	BufferObj->DataSize = CurrentCbvSrvDesc.CbMaterial.BufferSize;

	BufferObj->BufferData = new int8_t[CurrentCbvSrvDesc.CbMaterial.BufferSize];
	memset(BufferObj->BufferData, 0, CurrentCbvSrvDesc.CbMaterial.BufferSize);

	XMFLOAT4X4 Mat;
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		FMaterial* Material = Meshes[i]->GetMaterial();
		Mat = Material->GetMaterialTransform();
		XMMATRIX MatTransform = XMLoadFloat4x4(&Mat);
		FMaterialConstants MatConstants;
		MatConstants.DiffuseAlbedo = Material->GetDiffuseAlbedo();
		MatConstants.FresnelR0 = Material->GetFresnelR0();
		MatConstants.Roughness = Material->GetRoughness();
		XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(MatTransform));

		memcpy(BufferObj->BufferData + i * CurrentCbvSrvDesc.CbMaterial.BufferSize / CurrentCbvSrvDesc.CbMaterial.BufferViewCount, &MatConstants, sizeof(MatConstants));
	}
	FD3DConstantBuffer* Cb = ConstantBuffers[BufferObj->Type];
	if (Cb)
	{
		Cb->UpdateConstantBufferInfo(BufferObj->BufferData, BufferObj->DataSize);
	}
	delete BufferObj;
}

void FD3D12DynamicRHI::UpdateScenePassConstants(FScene* RenderScene)
{
	const std::vector<FMesh*>& Meshes = RenderScene->GetDrawMeshes();
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_PASSCONSTANT;
	BufferObj->DataSize = CurrentCbvSrvDesc.CbConstant.BufferSize;

	BufferObj->BufferData = new int8_t[CurrentCbvSrvDesc.CbConstant.BufferSize];
	memset(BufferObj->BufferData, 0, CurrentCbvSrvDesc.CbConstant.BufferSize);

	LCamera& Camera = RenderScene->GetCamera();
	XMFLOAT4X4 MtProj;
	XMStoreFloat4x4(&MtProj, Camera.GetProjectionMatrix());
	XMMATRIX ViewProj = Camera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
	XMStoreFloat4x4(&PassConstant.ViewProj, XMMatrixTranspose(ViewProj));
	PassConstant.EyePosW = Camera.GetCameraLocation();
	FLight* Light = RenderScene->GetLight(0);
	PassConstant.Lights[0].Direction = Light->Direction; 
	PassConstant.Lights[0].Strength = Light->Strength;
	PassConstant.Lights[0].Position = Light->Position;

	XMFLOAT3 LightUp = {0, 0, 1};
	XMMATRIX LightView = XMMatrixLookToLH(XMLoadFloat3(&Light->Position), XMLoadFloat3(&Light->Direction), XMLoadFloat3(&LightUp));
	XMMATRIX LightProj = XMMatrixOrthographicLH((float)50, (float)50, 1.f, 100.f);
	XMMATRIX LightSpaceMatrix = LightView * LightProj;

	XMStoreFloat4x4(&PassConstant.LightSpaceMatrix, XMMatrixTranspose(LightSpaceMatrix));

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	XMMATRIX S = LightView * LightProj * T;
	XMStoreFloat4x4(&PassConstant.ShadowTransform, XMMatrixTranspose(S));
	

	memcpy(BufferObj->BufferData, &PassConstant, sizeof(PassConstant));
	FD3DConstantBuffer* Cb = ConstantBuffers[BufferObj->Type];
	if (Cb)
	{
		Cb->UpdateConstantBufferInfo(BufferObj->BufferData, BufferObj->DataSize);
	}
	delete BufferObj;
}

void FD3D12DynamicRHI::CreateD3DResources()
{
	CreateDescripterHeap(CurrentCbvSrvDesc.NeedDesciptorCount + 2, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
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

	// create shader map resource
	ShaderMap = new FD3DShaderMap(D3DDevice.Get(), SHADOW_WIDTH, SHADOW_HEIGHT);
	ShaderMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(CbvSrvHeap->GetCPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.ShaderMapDesc.HeapOffsetStart, CbvSrvDescriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CurrentCbvSrvDesc.ShaderMapDesc.HeapOffsetStart, CbvSrvDescriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(DsvHeap->GetCPUDescriptorHandleForHeapStart(), 1, DsvDescriptorSize)
	);

}

void FD3D12DynamicRHI::CreatePipelineStateObject(FRHIPiplineStateInitializer& Initializer)
{
	if (PiplelineStateObjCache.find(Initializer.KeyName) != PiplelineStateObjCache.end())
		return;

	FD3DGraphicPipline* PsoObj = new FD3DGraphicPipline();

	CD3DX12_ROOT_PARAMETER RootParameters[6];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[6];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
	Ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);
	Ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	Ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	Ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[1].InitAsDescriptorTable(1, &Ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[2].InitAsDescriptorTable(1, &Ranges[2], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[3].InitAsDescriptorTable(1, &Ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameters[4].InitAsDescriptorTable(1, &Ranges[4], D3D12_SHADER_VISIBILITY_PIXEL);
	RootParameters[5].InitAsDescriptorTable(1, &Ranges[5], D3D12_SHADER_VISIBILITY_PIXEL);

	// Allow input layout and deny uneccessary access to certain pipeline stages.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc(_countof(RootParameters), RootParameters, (UINT)StaticSamplers.size(), StaticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> Signature;
	ComPtr<ID3DBlob> Error;

	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &Signature, &Error));
	ThrowIfFailed(D3DDevice.Get()->CreateRootSignature(0, Signature->GetBufferPointer(), Signature->GetBufferSize(), IID_PPV_ARGS(&PsoObj->RootSignature)));
	NAME_D3D12_OBJECT(PsoObj->RootSignature);

	CD3DX12_RASTERIZER_DESC rasterizerStateDesc(D3D12_DEFAULT);
	rasterizerStateDesc.FrontCounterClockwise = Initializer.RasterizerStat.FrontCounterClockwise;
	rasterizerStateDesc.DepthBias = Initializer.RasterizerStat.DepthBias;
	rasterizerStateDesc.DepthBiasClamp = Initializer.RasterizerStat.DepthBiasClamp;
	rasterizerStateDesc.SlopeScaledDepthBias = Initializer.RasterizerStat.SlopeScaledDepthBias;
	auto ConvCullMode = [](FCullMode Mode) -> D3D12_CULL_MODE
	{
		switch (Mode)
		{
		case FCullMode::CULL_MODE_NONE: { return D3D12_CULL_MODE_NONE; }
		case FCullMode::CULL_MODE_FRONT: { return D3D12_CULL_MODE_FRONT; }
		case FCullMode::CULL_MODE_BACK: { return D3D12_CULL_MODE_BACK; }
		default: {return D3D12_CULL_MODE_NONE; }
		}
	};
	rasterizerStateDesc.CullMode = ConvCullMode(Initializer.RasterizerStat.CullMode);

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

	for (UINT i = 0; i < Initializer.NumElements; i++)
	{
		FRHIInputElement* RHIDesc = (FRHIInputElement*)(Initializer.pInpueElement + i);
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

	PsoDesc.InputLayout = { InputElementDescs.data(), Initializer.NumElements };
	PsoDesc.pRootSignature = PsoObj->RootSignature.Get();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(Initializer.pVSPointer, Initializer.VsPointerLength);
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(Initializer.pPsPointer, Initializer.PsPointerLength);
	PsoDesc.RasterizerState = rasterizerStateDesc;
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = Initializer.NumRenderTargets;
	PsoDesc.RTVFormats[0] = Initializer.RtvFormat == FRtvFormat::FORMAT_UNKNOWN ? DXGI_FORMAT_UNKNOWN : DXGI_FORMAT_R8G8B8A8_UNORM;
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT_D32_FLOAT;// DXGI_FORMAT_D24_UNORM_S8_UINT;

	ComPtr<ID3D12PipelineState> PipelineState;
	ThrowIfFailed(D3DDevice.Get()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PsoObj->PipelineState)));
	NAME_D3D12_OBJECT(PsoObj->PipelineState);
	PiplelineStateObjCache[Initializer.KeyName] = PsoObj;
}

void FD3D12DynamicRHI::CreateResourceViewCreater(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	ResourceViewCreater = new FD3D12ResourceViewCreater(D3DDevice.Get());
	ResourceViewCreater->OnCreate(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);
}

void FD3D12DynamicRHI::CreateVertexAndIndexBufferView(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(D3DDevice.Get(), CommandList.Get());
	D3DVertexBuffer->InitGPUVertexBufferView(D3DDevice.Get(), CommandList.Get());
}

FResourceView* FD3D12DynamicRHI::CreateResourceView(FTexture** Texture, E_RESOURCE_VIEW_TYPE ViewType, UINT ViewCount) 
{
	FResourceView* ResView = new FD3D12ResourceView();

	FD3D12ResourceViewCreater* ViewCreater = dynamic_cast<FD3D12ResourceViewCreater*>(ResourceViewCreater);
	ViewCreater->AllocDescriptor(ViewCount, ViewType, ResView);
	for (UINT i = 0; i < ViewCount; i++)
	{
		FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*(Texture + i));

		FD3D12ResourceView* View = dynamic_cast<FD3D12ResourceView*>(ResView);
		D3DTex->InitGPUTextureView(D3DDevice.Get(), CommandList.Get(), View->GetCpu(i));
	}

	return ResView;
}