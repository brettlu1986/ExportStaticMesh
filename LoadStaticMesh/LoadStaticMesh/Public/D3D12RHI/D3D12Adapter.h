#pragma once

#include "stdafx.h"

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

class D3D12Adapter
{
public:
	D3D12Adapter(D3D12AdapterDesc& DescIn);
	virtual ~D3D12Adapter() { }

	void Initialize(D3D12DynamicRHI* RHI);
	void InitializeDevices();

private:
	D3D12AdapterDesc Desc;
	D3D12DynamicRHI* OwningRHI;
	ComPtr<ID3D12Device> RootDevice;
};