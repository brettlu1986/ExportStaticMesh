
#include "stdafx.h"
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FDefine.h"
#include "FD3D12Resource.h"
#include "LAssetDataLoader.h"


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

 /// /////////////////

 void FD3D12DynamicRHI::RHIInitWindow(UINT Width, UINT Height, void* Window) 
 {
	 ChosenAdapter->InitWindow(Width, Height, Window);
	 ChosenAdapter->SetViewPort(FRHIViewPort(static_cast<float>(Width), static_cast<float>(Height)));
	 FRHIScissorRect Rect = FRHIScissorRect(0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height));
	 ChosenAdapter->SetScissorRect(Rect);
	 ChosenAdapter->CreateSwapChain();
	 ChosenAdapter->CreateRenderTargets();
 }

 void FD3D12DynamicRHI::RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
 {
	 ChosenAdapter->CreateConstantBuffer(BufferSize, pDataFrom, DataSize);
 }

 void FD3D12DynamicRHI::RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize)
 {
	 ChosenAdapter->UpdateConstantBufferData(pUpdateData, DataSize);
 }

 FShader* FD3D12DynamicRHI::RHICreateShader(LPCWSTR  ShaderFile)
 {
	 UINT8* ShaderData = nullptr;
	 UINT ShaderLen = 0;
	 ThrowIfFailed(ReadDataFromFile(LAssetDataLoader::GetAssetFullPath(ShaderFile).c_str(), &ShaderData, &ShaderLen));
	 return new FShader(ShaderData, ShaderLen);
 }

 void FD3D12DynamicRHI::RHICreatePiplineStateObject(FShader* Vs, FShader* Ps)
 {
	 FRHIInputElement RHIInputElementDescs[] =
	 {
		 { "POSITION", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		 { "TEXCOORD", 0, ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	 };

	 FRHIPiplineStateInitializer RHIPsoInitializer = {
		RHIInputElementDescs,
		_countof(RHIInputElementDescs),
		Vs->GetShaderByteCode(),
		Vs->GetDataLength(),
		Ps->GetShaderByteCode(),
		Ps->GetDataLength(),
		1
	 };
	 ChosenAdapter->CreatePso(RHIPsoInitializer);
 }

 void FD3D12DynamicRHI::RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color)
 {
	 FD3D12CommandList CommandList = ChosenAdapter->GetCommandListManager()->GetCommandList(0);
	 CommandList.Reset();
	 ID3D12GraphicsCommandList* DefaultCommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	 DefaultCommandList->RSSetViewports(1, &(ChosenAdapter->ViewPort));
	 DefaultCommandList->RSSetScissorRects(1, &(ChosenAdapter->ScissorRect));

	 ChosenAdapter->InitRenderBegin(TargetFrame, Color);

	 FD3DConstantBuffer* ConstantBuffer = ChosenAdapter->GetConstantBuffer();
	 DefaultCommandList->SetGraphicsRootSignature(ChosenAdapter->GetRootSignature());
	 DefaultCommandList->SetPipelineState(ChosenAdapter->GetDefaultPiplineState());

	 ID3D12DescriptorHeap* ppHeaps[] = { ChosenAdapter->CbvSrvHeap.Get(), ChosenAdapter->SamplerHeap.Get() };
	 DefaultCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	 DefaultCommandList->SetGraphicsRootDescriptorTable(0, ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	 CD3DX12_GPU_DESCRIPTOR_HANDLE tex(ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	 tex.Offset(1, ConstantBuffer->GetCbvSrvUavDescriptorSize());
	 DefaultCommandList->SetGraphicsRootDescriptorTable(1, tex);
	 DefaultCommandList->SetGraphicsRootDescriptorTable(2, ChosenAdapter->SamplerHeap->GetGPUDescriptorHandleForHeapStart());
	
 }

 void FD3D12DynamicRHI::RHIRenderEnd(UINT TargetFrame)
 {
	 ChosenAdapter->RenderEnd(TargetFrame);

	 ID3D12GraphicsCommandList* DefaultCommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	 DefaultCommandList->Close();
	 ID3D12CommandQueue* CommandQueue = ChosenAdapter->GetD3DCommandQueue();
	 ID3D12CommandList* CmdsLists[] = { DefaultCommandList };
	 CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
	 IDXGISwapChain* SwapChain = ChosenAdapter->GetSwapChain();
	 SwapChain->Present(1, 0);
	 ChosenAdapter->WaitForPreviousFrame();
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

void FD3D12DynamicRHI::RHIDrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	FD3D12IndexBuffer* D3DIndexBuffer = static_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = static_cast<FD3D12VertexBuffer*>(VertexBuffer);

	ID3D12GraphicsCommandList* CommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	CommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	CommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
}

 void FD3D12DynamicRHI::RHIPresent()
 {
	 //first flush
	 ID3D12GraphicsCommandList* CommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	 CommandList->Close();
	 ID3D12CommandQueue* CommandQueue = ChosenAdapter->GetD3DCommandQueue();
	 ID3D12CommandList* CmdsLists[] = { CommandList };
	 CommandQueue->ExecuteCommandLists(_countof(CmdsLists), CmdsLists);
	 ChosenAdapter->SetFenceValue(1);
	 ChosenAdapter->WaitForPreviousFrame();
 }