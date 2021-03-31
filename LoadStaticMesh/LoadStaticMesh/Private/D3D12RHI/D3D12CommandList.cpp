
#include "D3D12CommandList.h"
#include "D3D12Device.h"
#include "D3D12CommandListAllocator.h"
#include "D3D12Helper.h"

D3D12CommandList::D3D12CommandList(D3D12Device* ParentDevice, D3D12CommandListAllocator* CommandAllocator, UINT InCommandListIndex)
:ParentDevice(ParentDevice)
,CurrentCommandAllocator(CommandAllocator)
,CommandListIndex(InCommandListIndex)
{
	ParentDevice->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CurrentCommandAllocator->GetD3DCommandAllocator(),
		nullptr, IID_PPV_ARGS(&CommandList));
	NAME_D3D12_OBJECT_WITHINDEX(CommandList, CommandListIndex);
	Close();
}

D3D12CommandList::~D3D12CommandList()
{
}

void D3D12CommandList::Clear()
{
	if(CurrentCommandAllocator)
	{
		CurrentCommandAllocator->Reset();
		delete CurrentCommandAllocator; 
		CurrentCommandAllocator = nullptr;
	}
	CommandList.Reset();
}

void D3D12CommandList::Close()
{
	CommandList->Close();
}

void D3D12CommandList::Reset()
{

}