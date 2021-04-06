#include "FD3D12CommandListManager.h"
#include "FD3D12CommandList.h"
#include "FD3D12Helper.h"
#include "FD3D12Adapter.h"

FD3D12CommandListManager::FD3D12CommandListManager(FD3D12Adapter* InParent)
:ParentAdapter(InParent)
, WriteIndex(0)
,ReadIndex(0)
{

}

FD3D12CommandListManager::~FD3D12CommandListManager()
{

}

void FD3D12CommandListManager::Clear()
{

	for (size_t i = 0; i < ComandLists.size(); ++i)
	{
		ComandLists[i]->Clear();
	}
	ComandLists.clear();
	CommandQueue.Reset();
}

void FD3D12CommandListManager::Initialize()
{
	ID3D12Device* D3DDevice = ParentAdapter->GetD3DDevice();

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	NAME_D3D12_OBJECT(CommandQueue);

}

void FD3D12CommandListManager::CreateCommandLists(UINT NumCommandLists)
{
	if(NumCommandLists > 0)
	{
		for (UINT i = 0; i < NumCommandLists; ++i)
		{
			FD3D12CommandList* List = new FD3D12CommandList(ParentAdapter, i);
			ComandLists.push_back(List);
		}
	}
}