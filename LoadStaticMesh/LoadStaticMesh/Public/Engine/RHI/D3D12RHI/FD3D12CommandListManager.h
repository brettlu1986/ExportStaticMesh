#pragma once

#include "stdafx.h"
#include "FD3D12CommandList.h"
#include "FDefine.h"
#include <thread>
#include <atomic>
using namespace std;
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

	ID3D12GraphicsCommandList* GetDefaultInitCommandList() {
		return DefaultCommandList->GetD3DCommandList();
	}

	inline void Enqueue(ComPtr<ID3D12GraphicsCommandList> CommandList)
	{
		while (IsFull()) { std::this_thread::sleep_for(500ns); }
		CommandList.CopyTo(&(ComandLists[WriteIndex]->GetD3DCommandList2()));
		WriteIndex = (WriteIndex + 1) % COMMAND_LIST_NUM ;
	}

	inline bool Dequeue(FD3D12CommandList** outValue)
	{
		if (IsEmpty())
			return false;

		*outValue = ComandLists[ReadIndex];
		ReadIndex = (ReadIndex + 1) % COMMAND_LIST_NUM;

		return true;
	}

	inline bool IsFull()
	{
		return ReadIndex == ((WriteIndex + 1) % COMMAND_LIST_NUM);
	}

	inline bool IsEmpty()
	{
		return ReadIndex == WriteIndex;
	}

private:

	ComPtr<ID3D12CommandQueue> CommandQueue;
	FD3D12Adapter* ParentAdapter;
	std::vector<FD3D12CommandList*> ComandLists;

	FD3D12CommandList* DefaultCommandList;

	std::atomic<int> ReadIndex;
	std::atomic<int> WriteIndex;
};