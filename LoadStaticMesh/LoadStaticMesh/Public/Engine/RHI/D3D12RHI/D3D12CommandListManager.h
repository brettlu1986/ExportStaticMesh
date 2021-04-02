#pragma once

#include "stdafx.h"
#include "D3D12CommandList.h"
using namespace Microsoft::WRL;

class D3D12Device;
class D3D12CommandListManager
{
public:
	D3D12CommandListManager(D3D12Device* InParent);
	virtual ~D3D12CommandListManager();
	void Initialize();
	void Clear();
	void CreateCommandLists(UINT NumCommandLists);

	D3D12CommandList& GetCommandList(UINT Index)
	{
		return ComandLists[Index];
	}
	ID3D12GraphicsCommandList* GetDefaultCommandList() {
		return ComandLists[0].GetD3DCommandList();
	}
	ID3D12CommandQueue* GetD3DCommandQueue() const { return CommandQueue.Get(); }
private:

	ComPtr<ID3D12CommandQueue> CommandQueue;
	D3D12Device* ParentDevice;
	std::vector<D3D12CommandList> ComandLists;

};