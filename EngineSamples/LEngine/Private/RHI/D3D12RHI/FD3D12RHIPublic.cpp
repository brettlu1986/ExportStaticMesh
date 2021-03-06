
#include "pch.h"

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
#include "pix.h"

using namespace Microsoft::WRL;

static const FRHIColor ClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

FDynamicRHI* FD3D12DynamicRHIModule::CreateRHI()
{
	return new FD3D12DynamicRHI();
}

/////////////////////////// dynamic RHI implement
FD3D12DynamicRHI::FD3D12DynamicRHI()
:FenceEvent(HANDLE())
,FenceValues(0)
,FrameIndex(0)
,Window(HWND())
,WndHeight(0)
,WndWidth(0)
,DefaultViewPort(0.f, 0.f, 0.f, 0.f)
{
	
}

void FD3D12DynamicRHI::Init()
{
	FindAdapter();
	InitializeDevices();
	CreateSampler();
	LDeviceWindows* DeviceWindows = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	InitWindow(DeviceWindows->GetWidth(), DeviceWindows->GetHeight(), DeviceWindows->GetHwnd());
}

void FD3D12DynamicRHI::ShutDown()
{
	WaitForPreviousFrame();
	CloseHandle(FenceEvent);
	GpuFence.Reset();
	SwapChain.Reset();

	for (auto Pso : PiplelineStateObjCache)
	{
		Pso.second->Destroy();
	}
	PiplelineStateObjCache.clear();

	ResourceViewCreater->OnDestroy();
	delete ResourceViewCreater;
	ResourceViewCreater = nullptr;

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

void FD3D12DynamicRHI::BeginRenderScene()
{
	ThrowIfFailed(CommandAllocator->Reset());
	ThrowIfFailed(CommandList->Reset(CommandAllocator.Get(), nullptr));
}

void FD3D12DynamicRHI::EndRenderScene()
{
	vector<ID3D12CommandList*> CmdLists;
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

void FD3D12DynamicRHI::UpdateVertexBufferResource(FVertexBuffer* VertexBuffer, LVertexBuffer& VertexBufferData)
{
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DVertexBuffer->InitGPUVertexBufferView(D3DDevice.Get(), CommandList.Get(), VertexBufferData);
}

void FD3D12DynamicRHI::UpdateIndexBufferResource(FIndexBuffer* IndexBuffer, LIndexBuffer& IndexBufferData)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(D3DDevice.Get(), CommandList.Get(), IndexBufferData);
}

FShader* FD3D12DynamicRHI::RHICreateShader(string RefShaderName)
{
	LShader* ShaderData = LAssetManager::Get()->GetShader(RefShaderName);
	return new FShader(ShaderData->GetShaderByteCode(), ShaderData->GetDataLength());
}

FTexture* FD3D12DynamicRHI::CreateTexture(LTexture* TextureData)
{
	FTexture* Tex = new FD3D12Texture(D3DDevice.Get());
	Tex->InitializeTexture(TextureData);
	return Tex;
}

FTexture* FD3D12DynamicRHI::CreateTexture(FTextureInitializer TexInitializer)
{
	FTexture* Tex = new FD3D12Texture(D3DDevice.Get());
	Tex->InitializeTexture(TexInitializer);
	return Tex;
}

void FD3D12DynamicRHI::CreateResourceViewCreater(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount)
{
	ResourceViewCreater = new FD3D12ResourceViewCreater(D3DDevice.Get());
	ResourceViewCreater->OnCreate(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);
}


FResourceView* FD3D12DynamicRHI::CreateResourceView(FResourceViewInfo ViewInfo)
{
	FD3D12ResourceViewCreater* ViewCreater = dynamic_cast< FD3D12ResourceViewCreater*>(ResourceViewCreater);
	if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV)
	{
		FResourceView* ResView = new FD3D12ResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, ResView);
		for (UINT i = 0; i < ViewInfo.ViewCount; i++)
		{
			FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*(ViewInfo.TexResource + i));
			FD3D12ResourceView* View = dynamic_cast<FD3D12ResourceView*>(ResView);
			D3DTex->InitGPUTextureView(D3DDevice.Get(), CommandList.Get(), View->GetCpu(i), static_cast<DXGI_FORMAT>(ViewInfo.Format));
			View->SetViewResource(D3DTex->Resource());
			View->SetResourceState(D3DTex->GetInitState());
		}
		return ResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV)
	{
		FResourceView* CbResView = new FD3D12CbvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, CbResView);

		FD3D12CbvResourceView* View = dynamic_cast<FD3D12CbvResourceView*>(CbResView);
		View->SetConstantBufferViewInfo(D3DDevice.Get(), ViewInfo.DataSize);
		return CbResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV)
	{
		FResourceView* DsvResView = new FD3D12DsvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, DsvResView);

		FD3D12DsvResourceView* View = dynamic_cast<FD3D12DsvResourceView*>(DsvResView);
		FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*ViewInfo.TexResource);
		View->SetDsvViewInfo(D3DDevice.Get(), D3DTex, static_cast<DXGI_FORMAT>(ViewInfo.Format));
		View->SetResourceState(D3DTex->GetInitState());
		return DsvResView;
	}
	else if (ViewInfo.ViewType == E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV)
	{
		FResourceView* RtvResView = new FD3D12RtvResourceView();
		ViewCreater->AllocDescriptor(ViewInfo.ViewCount, ViewInfo.ViewType, RtvResView);
		FD3D12RtvResourceView* View = dynamic_cast<FD3D12RtvResourceView*>(RtvResView);
		if (ViewInfo.TexResource == nullptr)
		{
			View->SetRtvViewInfo(D3DDevice.Get(), SwapChain.Get(), static_cast<DXGI_FORMAT>(ViewInfo.Format), ViewInfo.Index);
			//swapchain back buffer init state is present
			View->SetResourceState(E_RESOURCE_STATE::RESOURCE_STATE_PRESENT);
		}
		else
		{
			FD3D12Texture* D3DTex = dynamic_cast<FD3D12Texture*>(*ViewInfo.TexResource);
			View->SetRtvViewInfo(D3DDevice.Get(), D3DTex, static_cast<DXGI_FORMAT>(ViewInfo.Format));
			View->SetResourceState(D3DTex->GetInitState());
		}
		return RtvResView;
	}
	assert(!"not valid resource view type");
	return new FResourceView();
}

void FD3D12DynamicRHI::UpdateConstantBufferView(FResourceView* CbvView, void* pDataUpdate)
{
	FD3D12CbvResourceView* CbvResView = dynamic_cast<FD3D12CbvResourceView*>(CbvView);
	CbvResView->UpdateConstantBufferInfo(pDataUpdate);
}

void FD3D12DynamicRHI::CreatePipelineStateObject(FPiplineStateInitializer Initializer)
{
	if (PiplelineStateObjCache.find(Initializer.KeyName) != PiplelineStateObjCache.end())
		return;

	FD3DGraphicPipline* PsoObj = new FD3DGraphicPipline();

	CD3DX12_ROOT_PARAMETER RootParameters[6];
	ZeroMemory(RootParameters, sizeof(RootParameters));
	CD3DX12_DESCRIPTOR_RANGE Ranges[6];
	ZeroMemory(Ranges, sizeof(Ranges));
	Ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); //matrix constant
	Ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1); //material constant & skeleton matrix palette
	Ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2); //pass ViewProj
	Ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 3); //pass Light
	Ranges[4].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); //object diffuse texture
	Ranges[5].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1); //shadow map  

	RootParameters[0].InitAsDescriptorTable(1, &Ranges[0], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[1].InitAsDescriptorTable(1, &Ranges[1], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[2].InitAsDescriptorTable(1, &Ranges[2], D3D12_SHADER_VISIBILITY_ALL);
	RootParameters[3].InitAsDescriptorTable(1, &Ranges[3], D3D12_SHADER_VISIBILITY_ALL);
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
	rasterizerStateDesc.CullMode = static_cast<D3D12_CULL_MODE>(Initializer.RasterizerStat.CullMode);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc = {};
	ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	vector<D3D12_INPUT_ELEMENT_DESC> InputElementDescs;

	for (UINT i = 0; i < Initializer.NumElements; i++)
	{
		FRHIInputElement* RHIDesc = (FRHIInputElement*)(Initializer.pInputElement + i);
		D3D12_INPUT_ELEMENT_DESC D3DInputDesc;
		D3DInputDesc.SemanticName = RHIDesc->SemanticName.c_str();
		D3DInputDesc.SemanticIndex = RHIDesc->SemanticIndex;
		D3DInputDesc.Format = static_cast<DXGI_FORMAT>(RHIDesc->Format);
		D3DInputDesc.InputSlot = RHIDesc->InputSlot;
		D3DInputDesc.AlignedByteOffset = RHIDesc->AlignedByteOffset;
		D3DInputDesc.InputSlotClass = static_cast<D3D12_INPUT_CLASSIFICATION>(RHIDesc->InputSlotClass);
		D3DInputDesc.InstanceDataStepRate = RHIDesc->InstanceDataStepRate;
		InputElementDescs.push_back(D3DInputDesc);
	}

	FShader* Vs = RHICreateShader(Initializer.RefVsShader);
	FShader* Ps = RHICreateShader(Initializer.RefPsShader);

	PsoDesc.InputLayout = { InputElementDescs.data(), Initializer.NumElements };
	PsoDesc.pRootSignature = PsoObj->RootSignature.Get();
	PsoDesc.VS = CD3DX12_SHADER_BYTECODE(Vs->GetShaderByteCode(), Vs->GetDataLength());
	PsoDesc.PS = CD3DX12_SHADER_BYTECODE(Ps->GetShaderByteCode(), Ps->GetDataLength());
	PsoDesc.RasterizerState = rasterizerStateDesc;	
	PsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	PsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	PsoDesc.SampleMask = UINT_MAX;
	PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PsoDesc.NumRenderTargets = Initializer.NumRenderTargets;
	PsoDesc.RTVFormats[0] = static_cast<DXGI_FORMAT>(Initializer.RtvFormat);
	PsoDesc.SampleDesc.Count = 1;
	PsoDesc.SampleDesc.Quality = 0;	////not use 4XMSAA
	PsoDesc.DSVFormat = static_cast<DXGI_FORMAT>(Initializer.DsvFormat);
	if(Initializer.DsvFormat == E_GRAPHIC_FORMAT::FORMAT_UNKNOWN)
	{
		PsoDesc.DepthStencilState.DepthEnable = false;
	}
	if (Initializer.bTransparency)
	{
		D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
		transparencyBlendDesc.BlendEnable = true;
		transparencyBlendDesc.LogicOpEnable = false;
		transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
		transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
		transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		PsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
	}

	ComPtr<ID3D12PipelineState> PipelineState;
	ThrowIfFailed(D3DDevice.Get()->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&PsoObj->PipelineState)));
	NAME_D3D12_OBJECT(PsoObj->PipelineState);
	PiplelineStateObjCache[Initializer.KeyName] = PsoObj;

	delete Vs;
	delete Ps;
}

void FD3D12DynamicRHI::SetResourceHeaps(vector<FResourceHeap*>& Heaps)
{
	vector <ID3D12DescriptorHeap*> _Heaps;
	for (size_t i = 0; i < Heaps.size(); i++)
	{
		FD3D12ResourceViewHeap* Heap = dynamic_cast<FD3D12ResourceViewHeap*>(Heaps[i]);
		_Heaps.push_back(Heap->GetHeap());
	}
	CommandList->SetDescriptorHeaps((UINT)_Heaps.size(), _Heaps.data());
}

void FD3D12DynamicRHI::SetRenderTargets(FResourceView* RtvView, FResourceView* DsvView)
{
	D3D12_CPU_DESCRIPTOR_HANDLE* pRtvHandle = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE* pDsvHandle = nullptr;

	D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE DsvHandle;

	if(RtvView != nullptr)
	{
		FD3D12RtvResourceView* Rtv = dynamic_cast<FD3D12RtvResourceView*>(RtvView);
		float Color[4] = { ClearColor.R, ClearColor.G, ClearColor.B, ClearColor.A };
		RtvHandle = Rtv->GetCpu();
		CommandList->ClearRenderTargetView(RtvHandle, Color, 0, nullptr);
		pRtvHandle = &RtvHandle;
	}

	if(DsvView != nullptr)
	{
		FD3D12DsvResourceView* Dsv = dynamic_cast<FD3D12DsvResourceView*>(DsvView);
		DsvHandle = Dsv->GetCpu();
		CommandList->ClearDepthStencilView(DsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		pDsvHandle = &DsvHandle;
	}

	UINT RtvCount = pRtvHandle == nullptr ? 0 : 1;

	bool bNotValid = pDsvHandle == nullptr || pRtvHandle == nullptr;
	bool bRtsSingleHandle = !bNotValid;
	
	CommandList->OMSetRenderTargets(RtvCount, pRtvHandle, bRtsSingleHandle, pDsvHandle);
}

void FD3D12DynamicRHI::SetVertexAndIndexBuffers(FVertexBuffer* VertexBuffer, FIndexBuffer* IndexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
}

void FD3D12DynamicRHI::SetPiplineStateObject(FD3DGraphicPipline* PsoObj)
{
	CommandList->SetGraphicsRootSignature(PsoObj->RootSignature.Get());
	CommandList->SetPipelineState(PsoObj->PipelineState.Get());
}

void FD3D12DynamicRHI::SetResourceParams(UINT Index, FResourceView* ResView)
{
	FD3D12ResourceView* View = dynamic_cast<FD3D12ResourceView*>(ResView);
	CommandList->SetGraphicsRootDescriptorTable(Index, View->GetGpu());
}

void FD3D12DynamicRHI::DrawTriangleList(FIndexBuffer* IndexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
}

void FD3D12DynamicRHI::ResourceTransition(FResourceView* ResourceView, E_RESOURCE_STATE StateTo)
{
	FD3D12ResourceView* ResView = dynamic_cast<FD3D12ResourceView*>(ResourceView);
	ID3D12Resource* Resource = ResView->Resource();
	
	if (Resource)
	{
		E_RESOURCE_STATE CurrentState = ResView->GetResourceState();
		if(CurrentState != StateTo)
		{
			const D3D12_RESOURCE_BARRIER Rb1 = CD3DX12_RESOURCE_BARRIER::Transition(Resource, static_cast<D3D12_RESOURCE_STATES>(CurrentState), static_cast<D3D12_RESOURCE_STATES>(StateTo));
			CommandList->ResourceBarrier(1, &Rb1);
			ResView->SetResourceState(StateTo);
		}
	}
	else
	{
		assert(!"resource should be valid!");
	}
}

void FD3D12DynamicRHI::BeginEvent(const char* Name)
{
	PIXBeginEvent(CommandList.Get(), 0, Name);
}

void FD3D12DynamicRHI::EndEvent()
{
	PIXEndEvent(CommandList.Get());
}

void FD3D12DynamicRHI::SetViewPortInfo(FRHIViewPort ViewPort)
{
	CD3DX12_VIEWPORT Viewport = CD3DX12_VIEWPORT(ViewPort.Left,
		ViewPort.Top,
		ViewPort.Width,
		ViewPort.Height);
	CD3DX12_RECT ScissorRect = CD3DX12_RECT(static_cast<LONG>(ViewPort.Left),
		static_cast<LONG>(ViewPort.Top),
		static_cast<LONG>(ViewPort.Left + ViewPort.Width),
		static_cast<LONG>(ViewPort.Top + ViewPort.Height));

	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);
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

void FD3D12DynamicRHI::InitWindow(UINT Width, UINT Height, void* InWindow)
{
	WndWidth = Width;
	WndHeight = Height;
	Window = (HWND)InWindow;
	DefaultViewPort = FRHIViewPort(0.f, 0.f, static_cast<float>(Width), static_cast<float>(Height));

	CreateSwapChain();
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
		FenceValues = 1;
		NAME_D3D12_OBJECT(GpuFence);
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	}
}

void FD3D12DynamicRHI::CreateSampler()
{
	CD3DX12_STATIC_SAMPLER_DESC ShadowSampler(
		0, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	CD3DX12_STATIC_SAMPLER_DESC TexSampler 
	{
		1,  
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP, 
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  
		0.f,  
		8, 
		D3D12_COMPARISON_FUNC_LESS_EQUAL, 
	};

	CD3DX12_STATIC_SAMPLER_DESC BloomSampler1
	{
		2,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.f,
		1,
		D3D12_COMPARISON_FUNC_ALWAYS,
	};

	CD3DX12_STATIC_SAMPLER_DESC BloomSampler2
	{
		3,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.f,
		1,
		D3D12_COMPARISON_FUNC_NEVER,
	};

	CD3DX12_STATIC_SAMPLER_DESC BloomSampler3
	{
		4,
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.f,
		1,
		D3D12_COMPARISON_FUNC_NEVER,
	};
	StaticSamplers.push_back(ShadowSampler);
	StaticSamplers.push_back(TexSampler);
	StaticSamplers.push_back(BloomSampler1);
	StaticSamplers.push_back(BloomSampler2);
	StaticSamplers.push_back(BloomSampler3);
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
