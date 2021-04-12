#pragma once

#include "stdafx.h"
#include "FDynamicRHI.h"
#include "FD3D12CommandListManager.h"
#include <map>

using namespace Microsoft::WRL;

struct FD3D12AdapterDesc
{
	FD3D12AdapterDesc()
		:AdapterIndex(-1)
		,MaxSupportedFeatureLevel((D3D_FEATURE_LEVEL)0)
		,NumDeviceNodes(0)
		, Desc()
	{
	}

	FD3D12AdapterDesc(DXGI_ADAPTER_DESC1& DescIn, INT32 InAdapterIndex, D3D_FEATURE_LEVEL InMaxSupportedFeatureLevel, UINT NumNodes)
		: AdapterIndex(InAdapterIndex)
		, MaxSupportedFeatureLevel(InMaxSupportedFeatureLevel)
		, Desc(DescIn)
		, NumDeviceNodes(NumNodes)
	{
	}

	INT32 AdapterIndex;
	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel;
	DXGI_ADAPTER_DESC1 Desc;
	UINT NumDeviceNodes;
};

class FD3D12DynamicRHI; 
class FD3DConstantBuffer;

class FD3D12Adapter
{
public:
	FD3D12Adapter(FD3D12AdapterDesc& DescIn);
	virtual ~FD3D12Adapter() { }

	void ShutDown();

	void Initialize(FD3D12DynamicRHI* RHI);

	FD3D12DynamicRHI* GetOwningRHI() const { return OwningRHI; }
	IDXGIAdapter1* GetAdapter() const { return DxgiAdapter.Get(); }

	ID3D12Device* GetD3DDevice() const { return D3DDevice.Get(); }

	ID3D12RootSignature* GetRootSignature() const {
		return RootSignature.Get();
	}
	IDXGISwapChain* GetSwapChain() const 
	{
		return SwapChain.Get();
	}

	void CreateConstantBuffer(UINT BufferSize, UINT BufferViewNum);
	void UpdateConstantBufferData(void* pUpdateData, UINT DataSize);
	FD3DConstantBuffer* GetConstantBuffer() const
	{
		return ConstantBuffer;
	}
	FD3D12CommandListManager* GetCommandListManager() const {
		return CommandListManager;
	}

	ID3D12CommandQueue* GetD3DCommandQueue() const
	{
		return GetCommandListManager()->GetD3DCommandQueue();
	}

	void InitWindow(UINT Width, UINT Height, void* Windwow);

	void SetViewPort(const FRHIViewPort& InViewPort);
	void SetScissorRect(const FRHIScissorRect& InRect);

	void CreateRenderTargets();
	void InitRenderBegin(ID3D12GraphicsCommandList* CommandList, UINT TargetFrame, FRHIColor Color);
	void RenderEnd(ID3D12GraphicsCommandList* CommandList, UINT TargetFrame);

	void CreateSwapChain();
	void CreatePso(const FRHIPiplineStateInitializer& PsoInitializer, bool bDefaultPso);

	void SetFenceValue(UINT64 Value) { FenceValue  = Value;}
	void WaitForPreviousFrame();

	ID3D12PipelineState* GetDefaultPiplineState() const 
	{
		return DefaultPso.Get();
	}

	ID3D12PipelineState* GetPiplineState(const std::string& PsoKey)
	{
		return PiplelineStateCache[PsoKey].Get();
	}

	UINT GetSrvOffsetInHeap() const 
	{
		return OffsetSrvInHeap;
	}

	UINT GetConstantBufferViewCount() const
	{
		return ConstantBufferViewNum;
	}

	ComPtr<ID3D12DescriptorHeap> RtvHeap;
	ComPtr<ID3D12DescriptorHeap> DsvHeap;
	ComPtr<ID3D12DescriptorHeap> CbvSrvHeap;
	ComPtr<ID3D12DescriptorHeap> SamplerHeap;
	
	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

private:
	void InitializeDevices();
	void CreateDescriptorHeaps();
	void CreateConstantBufferView(UINT BufferViewNum);

	void CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type,
		D3D12_DESCRIPTOR_HEAP_FLAGS, UINT NodeMask, REFIID riid, _COM_Outptr_  void** ppvHeap);
	
	void CreateSignature();
	void CreateSampler();

	UINT WndWidth;
	UINT WndHeight;
	HWND Window;

	FD3D12AdapterDesc Desc;
	FD3D12DynamicRHI* OwningRHI;

	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGIAdapter1> DxgiAdapter;
	ComPtr<IDXGISwapChain> SwapChain;

	ComPtr<ID3D12RootSignature> RootSignature;
	std::map<std::string, ComPtr<ID3D12PipelineState>> PiplelineStateCache;
	ComPtr <ID3D12PipelineState> DefaultPso;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	ComPtr<ID3D12Resource> RenderTargets[RENDER_TARGET_COUNT];
	ComPtr<ID3D12Resource> DepthStencilBuffer;

	FD3DConstantBuffer* ConstantBuffer;

	UINT OffsetSrvInHeap;
	UINT ConstantBufferViewNum;

	FD3D12CommandListManager* CommandListManager;
	ComPtr<ID3D12Fence> GpuFence;
	HANDLE FenceEvent;
	UINT64 FenceValue;
};