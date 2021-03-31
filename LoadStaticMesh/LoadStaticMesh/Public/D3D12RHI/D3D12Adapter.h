#pragma once

#include "stdafx.h"
#include "DynamicRHI.h"

using namespace Microsoft::WRL;

struct D3D12AdapterDesc
{
	D3D12AdapterDesc()
		:AdapterIndex(-1)
		,MaxSupportedFeatureLevel((D3D_FEATURE_LEVEL)0)
		,NumDeviceNodes(0)
		, Desc()
	{
	}

	D3D12AdapterDesc(DXGI_ADAPTER_DESC1& DescIn, INT32 InAdapterIndex, D3D_FEATURE_LEVEL InMaxSupportedFeatureLevel, UINT NumNodes)
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

class D3D12DynamicRHI; 
class D3D12Fence;
class D3D12Device;

class D3D12Adapter
{
public:
	D3D12Adapter(D3D12AdapterDesc& DescIn);
	virtual ~D3D12Adapter() { }

	void ShutDown();

	void Initialize(D3D12DynamicRHI* RHI);

	D3D12DynamicRHI* GetOwningRHI() const { return OwningRHI; }
	IDXGIAdapter1* GetAdapter() const { return DxgiAdapter.Get(); }

	ID3D12Device* GetD3DDevice() const { return D3DDevice.Get(); }
	D3D12Device* GetDevice() const { return Device;}

	void SetViewPort(const RHIViewPort& InViewPort);
	void SetScissorRect(const RHIScissorRect& InRect);

	void CreateSwapChain(const RHISwapObjectInfo& SwapInfo);
private:
	void InitializeDevices();

	D3D12AdapterDesc Desc;
	D3D12DynamicRHI* OwningRHI;

	CD3DX12_VIEWPORT ViewPort;
	CD3DX12_RECT ScissorRect;

	D3D12Fence* Fence;
	D3D12Device* Device;
	ComPtr<ID3D12Device> D3DDevice;
	ComPtr<IDXGIFactory4> DxgiFactory;
	ComPtr<IDXGIAdapter1> DxgiAdapter;
	ComPtr<IDXGISwapChain> SwapChain;
};