
#include "stdafx.h"
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FD3D12Device.h"
#include "FDefine.h"
#include "FD3D12Resource.h"


using namespace Microsoft::WRL;
using namespace DirectX;


bool FD3D12DynamicRHIModule::IsSupported()
{
	if(ChosenAdapter == nullptr)
	{
		FindAdapter();
	}
	return ChosenAdapter != nullptr;
}

FDynamicRHI* FD3D12DynamicRHIModule::CreateRHI()
{
	return new FD3D12DynamicRHI(ChosenAdapter);
}

void FD3D12DynamicRHIModule::FindAdapter()
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
	FD3D12AdapterDesc CurrentAdapter(Desc, Index, MaxSupportedFeatureLevel, pDevice->GetNodeCount());
	ChosenAdapter = new FD3D12Adapter(CurrentAdapter);
	pDevice->Release();
}

/////////////////////////// dynamic RHI implement
FD3D12DynamicRHI::FD3D12DynamicRHI(FD3D12Adapter* ChosenAdapterIn)
{
	ChosenAdapter = ChosenAdapterIn;
}

void FD3D12DynamicRHI::Init() 
{
	if(ChosenAdapter)
	{
		ChosenAdapter->Initialize(this);
	}
}

void FD3D12DynamicRHI::ShutDown()
{
	if(ChosenAdapter)
	{
		ChosenAdapter->ShutDown();
		ChosenAdapter = nullptr;
	}
}


FGenericFence* FD3D12DynamicRHI::RHICreateFence(const std::string& Name) 
{
	return new FD3D12Fence(Name, ChosenAdapter);
}


void FD3D12DynamicRHI::RHICreateViewPort(FRHIViewPort& ViewPort)
{
	ChosenAdapter->SetViewPort(ViewPort);
	FRHIScissorRect Rect = FRHIScissorRect(0, 0, static_cast<LONG>(ViewPort.Width), static_cast<LONG>(ViewPort.Height));
	ChosenAdapter->SetScissorRect(Rect);
}

void FD3D12DynamicRHI::RHICreateSwapObject(FRHISwapObjectInfo& SwapInfo) 
{
	ChosenAdapter->CreateSwapChain(SwapInfo);
}

void FD3D12DynamicRHI::RHICreatePiplineStateObject(FRHIPiplineStateInitializer& Initializer) 
{
	ChosenAdapter->CreatePso(Initializer);
}

void FD3D12DynamicRHI::RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size)
{
	ThrowIfFailed(ReadDataFromFile(FileName.c_str(), Data, Size));
}


FRHICommandList& FD3D12DynamicRHI::RHIGetCommandList(UINT Index)
{
	return ChosenAdapter->GetDevice()->GetCommandListManager()->GetCommandList(Index);
}

void FD3D12DynamicRHI::RHIExcuteCommandList(FRHICommandList& CommandList)
{
	CommandList.Close();
	CommandList.Excute();
}

void FD3D12DynamicRHI::RHICloseCommandList(FRHICommandList& CommandList)
{
	CommandList.Close();
}

void FD3D12DynamicRHI::RHISignalCurrentFence()
{
	FD3D12Fence* Fence = ChosenAdapter->GetFence();
	Fence->SignalCurrentFence();
}

bool FD3D12DynamicRHI::RHIIsFenceComplete()
{
	FD3D12Fence* Fence = ChosenAdapter->GetFence();
	return Fence->IsFenceComplete();
}

void FD3D12DynamicRHI::RHIResetCommandList(FRHICommandList& CommandList)
{
	CommandList.Reset();
}

void FD3D12DynamicRHI::RHISetCurrentViewPortAndScissorRect(FRHICommandList& CommandList)
{
	CommandList.SetViewPort();
	CommandList.SetScissorRect();
}


 void FD3D12DynamicRHI::RHISetGraphicRootDescripterTable(FRHICommandList& CommandList)
 {
	 CommandList.SetGraphicRootDescripterTable();
 }


 void FD3D12DynamicRHI::RHISwapObjectPresent()
 {
	 IDXGISwapChain* SwapChain = ChosenAdapter->GetSwapChain();
	 SwapChain->Present(1, 0);
 }


 /// /////////////////


 void FD3D12DynamicRHI::RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
 {
	 ChosenAdapter->CreateConstantBuffer(BufferSize, pDataFrom, DataSize);
 }

 void FD3D12DynamicRHI::RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize)
 {
	 ChosenAdapter->UpdateConstantBufferData(pUpdateData, DataSize);
 }

 void FD3D12DynamicRHI::RHICreateRenderTarget(UINT Width, UINT Height)
 {
	 ChosenAdapter->CreateRenderTarget(Width, Height);
 }

 void FD3D12DynamicRHI::RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color)
 {
	 ChosenAdapter->InitRenderBegin(TargetFrame, Color);
 }

 void FD3D12DynamicRHI::RHIRenderEnd(UINT TargetFrame)
 {
	 ChosenAdapter->RenderEnd(TargetFrame);
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
	FD3D12IndexBuffer* D3DIndexBuffer = static_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = static_cast<FD3D12VertexBuffer*>(VertexBuffer);
	FD3D12Texture* D3DTexture = static_cast<FD3D12Texture*>(Texture);
	D3DIndexBuffer->InitGPUIndexBufferView(ChosenAdapter);
	D3DVertexBuffer->InitGPUVertexBufferView(ChosenAdapter);
	D3DTexture->InitGPUTextureView(ChosenAdapter);
}

void FD3D12DynamicRHI::RHIDrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* TextureRes)
{
	FD3D12IndexBuffer* D3DIndexBuffer = static_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = static_cast<FD3D12VertexBuffer*>(VertexBuffer);

	ID3D12GraphicsCommandList* CommandList = ChosenAdapter->GetDevice()->GetCommandListManager()->GetDefaultCommandList();
	CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
}