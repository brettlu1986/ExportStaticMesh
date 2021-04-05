#pragma once

#include "stdafx.h"
#include "FD3D12CommandList.h"
using namespace Microsoft::WRL;

class FD3D12Adapter;
class FD3D12CommandListManager
{
public:
	FD3D12CommandListManager(FD3D12Adapter* InAdapter);
	virtual ~FD3D12CommandListManager();
	void Initialize();
	void Clear();
	void CreateCommandLists(UINT NumCommandLists);

	FD3D12CommandList& GetCommandList(UINT Index)
	{
		return *(ComandLists[Index]);
	}
	ID3D12GraphicsCommandList* GetDefaultCommandList() {
		return GetCommandList(0).GetD3DCommandList();
	}
	ID3D12CommandQueue* GetD3DCommandQueue() const { return CommandQueue.Get(); }
private:

	ComPtr<ID3D12CommandQueue> CommandQueue;
	FD3D12Adapter* ParentAdapter;
	std::vector<FD3D12CommandList*> ComandLists;

};