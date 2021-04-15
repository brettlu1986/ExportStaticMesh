
#include "stdafx.h"

#include <dxgidebug.h>
#include <d3dcompiler.h>
#include "FD3D12RHIPrivate.h"
#include "FD3D12Helper.h"
#include "FDefine.h"
#include "FD3D12Resource.h"
#include "LAssetDataLoader.h"
#include "LEngine.h"
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
	FD3D12AdapterDesc CurrentAdapter(Desc, Index, MaxSupportedFeatureLevel, pDevice->GetNodeCount());
	ChosenAdapter = new FD3D12Adapter(CurrentAdapter);
	pDevice->Release();
}

bool FD3D12DynamicRHI::IsSupported()
{
	if (ChosenAdapter == nullptr)
	{
		FindAdapter();
	}
	return ChosenAdapter != nullptr;
}

void FD3D12DynamicRHI::Init() 
{
	if(!IsSupported())
		return;

	if(ChosenAdapter)
	{
		ChosenAdapter->Initialize(this);
		LDeviceWindows* DeviceWindows = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
		RHIInitWindow(DeviceWindows->GetWidth(), DeviceWindows->GetHeight(), DeviceWindows->GetHwnd());
	}
}

void FD3D12DynamicRHI::RHIInitWindow(UINT Width, UINT Height, void* Window)
{
	ChosenAdapter->InitWindow(Width, Height, Window);
	ChosenAdapter->SetViewPort(FRHIViewPort(static_cast<float>(Width), static_cast<float>(Height)));
	FRHIScissorRect Rect = FRHIScissorRect(0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height));
	ChosenAdapter->SetScissorRect(Rect);
	ChosenAdapter->CreateSwapChain();
	ChosenAdapter->CreateRenderTargets();
}

void FD3D12DynamicRHI::ShutDown()
{
	if(ChosenAdapter)
	{
		ChosenAdapter->ShutDown();
		ChosenAdapter = nullptr;
	}

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
	 ChosenAdapter->CreateSrvAndCbvs(Desc);
 }

 void FD3D12DynamicRHI::RHIUpdateConstantBuffer(void* pUpdateData)
 {
	FBufferObject* BuffObj = (FBufferObject*)(pUpdateData);
	FD3DConstantBuffer* Cb = ChosenAdapter->GetConstantBufferByCbType(BuffObj->Type);
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
	 ChosenAdapter->CreatePso(RHIPsoInitializer, bDefaultPso);
		
 }

 void FD3D12DynamicRHI::RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color)
 {
	 FD3D12CommandList CommandList = ChosenAdapter->GetCommandListManager()->GetCommandList(0);
	 CommandList.Reset();

	 ID3D12GraphicsCommandList* CurrentCommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	 CurrentCommandList->SetGraphicsRootSignature(ChosenAdapter->GetRootSignature());

	 ID3D12DescriptorHeap* ppHeaps[] = { ChosenAdapter->CbvSrvHeap.Get(), ChosenAdapter->SamplerHeap.Get() };
	 CurrentCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	 CurrentCommandList->RSSetViewports(1, &(ChosenAdapter->ViewPort));
	 CurrentCommandList->RSSetScissorRects(1, &(ChosenAdapter->ScissorRect));
	 ChosenAdapter->InitRenderBegin(CurrentCommandList, TargetFrame, Color);

	 // pass constant buffer
	 const FCbvSrvDesc& CbvSrvDesc = ChosenAdapter->GetCurrentCbvSrvDesc();
	 CD3DX12_GPU_DESCRIPTOR_HANDLE cbvSrvHandle(ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvDesc.CbConstant.HeapOffsetStart,
		 ChosenAdapter->GetCbvSrvUavDescriptorSize());
	 CurrentCommandList->SetGraphicsRootDescriptorTable(2, cbvSrvHandle);
 }

 void FD3D12DynamicRHI::RHIDrawMesh(FMesh* Mesh)
 {
	 ID3D12GraphicsCommandList* CurrentCommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();

	 FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(Mesh->GetIndexBuffer());
	 FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(Mesh->GetVertexBuffer());

	 CurrentCommandList->IASetVertexBuffers(0, 1, &(D3DVertexBuffer->GetVertexBufferView()));
	 CurrentCommandList->IASetIndexBuffer(&(D3DIndexBuffer->GetIndexBufferView()));
	 CurrentCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	 CurrentCommandList->SetPipelineState(ChosenAdapter->GetPiplineState(Mesh->GetPsoKey()));

	 const FCbvSrvDesc& CbvSrvDesc = ChosenAdapter->GetCurrentCbvSrvDesc();
	 CD3DX12_GPU_DESCRIPTOR_HANDLE MtHandle(ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvDesc.CbMatrix.HeapOffsetStart + Mesh->GetBufferIndex(),
		 ChosenAdapter->GetCbvSrvUavDescriptorSize());
	 CurrentCommandList->SetGraphicsRootDescriptorTable(0, MtHandle);

	 FMaterial* Material = Mesh->GetMaterial();
	 CD3DX12_GPU_DESCRIPTOR_HANDLE MatHandle(ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvDesc.CbMaterial.HeapOffsetStart + Material->GetMaterialCbvHeapIndex(),
		 ChosenAdapter->GetCbvSrvUavDescriptorSize());
	 CurrentCommandList->SetGraphicsRootDescriptorTable(1, MatHandle);

	 if(Mesh->GetDiffuseTexture())
	 {
		 FTexture* Tex = Mesh->GetDiffuseTexture();
		 CD3DX12_GPU_DESCRIPTOR_HANDLE TexHandle(ChosenAdapter->CbvSrvHeap->GetGPUDescriptorHandleForHeapStart(), CbvSrvDesc.SrvDesc.HeapOffsetStart + Tex->GetTextureHeapIndex(),
			 ChosenAdapter->GetCbvSrvUavDescriptorSize());
		 CurrentCommandList->SetGraphicsRootDescriptorTable(3, TexHandle);
		 CurrentCommandList->SetGraphicsRootDescriptorTable(4, ChosenAdapter->SamplerHeap->GetGPUDescriptorHandleForHeapStart());
	 }

	 CurrentCommandList->DrawIndexedInstanced(D3DIndexBuffer->GetIndicesCount(), 1, 0, 0, 0);
 }


 void FD3D12DynamicRHI::RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute)
 {
	 ID3D12GraphicsCommandList* CurrentCommandList = ChosenAdapter->GetCommandListManager()->GetDefaultCommandList();
	 ID3D12CommandQueue* CommandQueue = ChosenAdapter->GetD3DCommandQueue();
	 std::vector<ID3D12CommandList*> CmdLists;
	 if(bFirstExcute)
	 {
		 CurrentCommandList->Close();
		 CmdLists.push_back(CurrentCommandList);
		 CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
		 ChosenAdapter->SetFenceValue(1);
	 }
	 else 
	 {	
		 ChosenAdapter->RenderEnd(CurrentCommandList, TargetFrame);
		 CurrentCommandList->Close();
		 CmdLists.push_back(CurrentCommandList);
		 CommandQueue->ExecuteCommandLists(static_cast<UINT>(CmdLists.size()), CmdLists.data());
		 IDXGISwapChain* SwapChain = ChosenAdapter->GetSwapChain();
		 SwapChain->Present(1, 0);
	 }
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
	FD3D12IndexBuffer* D3DIndexBuffer = dynamic_cast<FD3D12IndexBuffer*>(IndexBuffer);
	FD3D12VertexBuffer* D3DVertexBuffer = dynamic_cast<FD3D12VertexBuffer*>(VertexBuffer);
	D3DIndexBuffer->InitGPUIndexBufferView(ChosenAdapter);
	D3DVertexBuffer->InitGPUVertexBufferView(ChosenAdapter);
	if(Texture)
	{
		FD3D12Texture* D3DTexture = dynamic_cast<FD3D12Texture*>(Texture);
		D3DTexture->InitGPUTextureView(ChosenAdapter);
	}
}



