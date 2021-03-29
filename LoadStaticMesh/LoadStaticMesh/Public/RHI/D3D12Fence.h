#pragma once

#include "stdafx.h"

class D3D12Adapter;
class D3D12Fence
{
public:
	D3D12Fence(D3D12Adapter* InParent);
	virtual ~D3D12Fence();

	virtual void CreateFence();
	virtual UINT64 Signal();

	bool IsFenceComplete(UINT64 FenceValue);
	void WaitForFence(UINT64 FenceValue);

};