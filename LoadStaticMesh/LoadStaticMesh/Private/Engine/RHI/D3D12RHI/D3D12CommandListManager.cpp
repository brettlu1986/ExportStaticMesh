#include "D3D12CommandListManager.h"
#include "D3D12Device.h"
#include "D3D12CommandListAllocator.h"
#include "D3D12CommandList.h"
#include "D3D12Helper.h"

D3D12CommandListManager::D3D12CommandListManager(D3D12Device* InParent)
:ParentDevice(InParent)
{

}

D3D12CommandListManager::~D3D12CommandListManager()
{

}

void D3D12CommandListManager::Clear()
{
	for (size_t i = 0; i < ComandLists.size(); ++i)
	{
		ComandLists[i].Clear();
	}
	ComandLists.clear();

	CommandQueue.Reset();
}

void D3D12CommandListManager::Initialize()
{
	D3D12Adapter* Adapter = ParentDevice->GetParentAdapter();
	ID3D12Device* D3DDevice = Adapter->GetD3DDevice();

	D3D12_COMMAND_QUEUE_DESC QueueDesc = {};
	QueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	ThrowIfFailed(D3DDevice->CreateCommandQueue(&QueueDesc, IID_PPV_ARGS(&CommandQueue)));
	NAME_D3D12_OBJECT(CommandQueue);

}

void D3D12CommandListManager::CreateCommandLists(UINT NumCommandLists)
{
	if(NumCommandLists > 0)
	{
		for (UINT i = 0; i < NumCommandLists; ++i)
		{
			D3D12CommandListAllocator* TempAlocator = new D3D12CommandListAllocator(ParentDevice->GetDevice(), i);
			D3D12CommandList List(ParentDevice, TempAlocator, i);
			ComandLists.push_back(List);
		}
	}
}