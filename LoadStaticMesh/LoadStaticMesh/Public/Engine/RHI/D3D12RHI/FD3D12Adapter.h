#pragma once

#include "stdafx.h"
#include "FDynamicRHI.h"

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
class FD3D12Fence;
class FD3D12Device;

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
	FD3D12Device* GetDevice() const { return Device;}
	FD3D12Fence* GetFence() const {return Fence;}

	ID3D12RootSignature* GetRootSignature() const {
		return RootSignature.Get();
	}
	IDXGISwapChain* GetSwapChain() const 
	{
		return SwapChain.Get();
	}


	void SetViewPort(const FRHIViewPort& InViewPort);
	void SetScissorRect(const FRHIScissorRect& InRect);

	void CreateRenderTarget(UINT Width, UINT Height);
	void InitRenderBegin(UINT TargetFrame, FRHIColor Color);
	void RenderEnd(UINT TargetFrame);

	void CreateSwapChain(const FRHISwapObjectInfo& SwapInfo);
	void CreatePso(const FRHIPiplineStateInitializer& PsoInitializer);

	ID3D12PipelineState* GetDefaultPiplineState() 
	{
		return PiplelineStateCache[0].Get();
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

	void CreateDescripterHeap(UINT NumDescripters, D3D12_DESCRIPTOR_HEAP_TYPE Type,
		D3D12_DESCRIPTOR_HEAP_FLAGS, UINT NodeMask, REFIID riid, _COM_Outptr_  void** ppvHeap);
	
	void CreateSignature();
	void CreateSampler();

	FD3D12AdapterDesc Desc;
	FD3D12DynamicRHI* OwningRHI;

	FD3D12Fence* Fence;
	FD3D12Device* Device;
	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGIAdapter1> DxgiAdapter;
	ComPtr<IDXGISwapChain> SwapChain;

	ComPtr<ID3D12RootSignature> RootSignature;
	std::vector<ComPtr<ID3D12PipelineState>> PiplelineStateCache;

	UINT RtvDescriptorSize;
	UINT DsvDescriptorSize;
	ComPtr<ID3D12Resource> RenderTargets[RENDER_TARGET_COUNT];
	ComPtr<ID3D12Resource> DepthStencilBuffer;
};