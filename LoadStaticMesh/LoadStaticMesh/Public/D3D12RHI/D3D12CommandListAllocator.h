#pragma once
#include "stdafx.h"

using namespace Microsoft::WRL;

class D3D12CommandListAllocator
{
public:
	D3D12CommandListAllocator(ID3D12Device* InDevice, UINT InAllocatorIndex);
	~D3D12CommandListAllocator();

	inline void Reset() { CommandAllocator->Reset(); }

	ID3D12CommandAllocator* GetD3DCommandAllocator() { return CommandAllocator.Get(); }
private: 
	void Init(ID3D12Device* InDevice, UINT InAllocatorIndex);

	UINT AllocatorIndex;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};