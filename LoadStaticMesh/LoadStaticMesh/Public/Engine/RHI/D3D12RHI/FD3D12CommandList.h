#pragma once
#include "stdafx.h"
#include "FRHIDef.h"

using namespace Microsoft::WRL;

class FD3D12Adapter;
class FD3D12CommandList 
{
public:
	FD3D12CommandList(FD3D12Adapter* InAdapter, UINT InCommandListIndex);

	~FD3D12CommandList();
	ID3D12GraphicsCommandList* GetD3DCommandList() 
	{
		return CommandList.Get();
	}

	void Clear() ;
	void Close() ;
	void Reset() ;

private: 
	FD3D12Adapter* ParentAdapter;
	UINT CommandListIndex;

	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};
