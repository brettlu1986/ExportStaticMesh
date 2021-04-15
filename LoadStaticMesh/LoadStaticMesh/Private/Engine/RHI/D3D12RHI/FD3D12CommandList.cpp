
#include "FD3D12CommandList.h"
#include "FD3D12Helper.h"
#include "FD3D12Resource.h"

FD3D12CommandList::FD3D12CommandList(ID3D12Device* InAdapter, UINT InCommandListIndex)
:ParentDevice(InAdapter)
{
	CommandListIndex = InCommandListIndex;

	ThrowIfFailed(ParentDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	NAME_D3D12_OBJECT_WITHINDEX(CommandAllocator, CommandListIndex);

	ParentDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&CommandList));
	NAME_D3D12_OBJECT_WITHINDEX(CommandList, CommandListIndex);
}

FD3D12CommandList::~FD3D12CommandList()
{
}

void FD3D12CommandList::Clear()
{
	CommandAllocator.Reset();
	CommandList.Reset();
}

void FD3D12CommandList::Close()
{
	CommandList->Close();
}

void FD3D12CommandList::Reset(ID3D12PipelineState* Pso)
{
	CommandAllocator->Reset();
	CommandList->Reset(CommandAllocator.Get(), Pso);
}