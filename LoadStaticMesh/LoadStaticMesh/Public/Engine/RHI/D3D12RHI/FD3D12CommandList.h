#pragma once
#include "stdafx.h"
#include "FRHIDef.h"

using namespace Microsoft::WRL;

class FD3D12CommandList 
{
public:
	FD3D12CommandList(ID3D12Device* InAdapter, UINT InCommandListIndex);

	~FD3D12CommandList();
	ID3D12GraphicsCommandList* GetD3DCommandList() 
	{
		return CommandList.Get();
	}

	ComPtr<ID3D12GraphicsCommandList> GetD3DCommandList2()
	{
		return CommandList;
	}

	void Clear() ;
	void Close() ;
	void Reset(ID3D12PipelineState* Pso) ;

private: 
	ID3D12Device* ParentDevice;
	UINT CommandListIndex;

	ComPtr<ID3D12GraphicsCommandList> CommandList;
	ComPtr<ID3D12CommandAllocator> CommandAllocator;
};
