#pragma once
#include "stdafx.h"

using namespace Microsoft::WRL;

class D3D12Device;
class D3D12CommandListAllocator;
class D3D12CommandList
{
public:
	D3D12CommandList(D3D12Device* ParentDevice, D3D12CommandListAllocator* CommandAllocator,UINT InCommandListIndex);

	~D3D12CommandList();

	void Clear();
	void Close();
	void Reset();
private: 
	UINT CommandListIndex;
	D3D12Device* ParentDevice;
	D3D12CommandListAllocator* CurrentCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> CommandList;
};
