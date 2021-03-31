#include "D3D12CommandListAllocator.h"
#include "D3D12Helper.h"

D3D12CommandListAllocator::D3D12CommandListAllocator(ID3D12Device* InDevice, UINT InAllocatorIndex)
{
	Init(InDevice, InAllocatorIndex);
}

D3D12CommandListAllocator::~D3D12CommandListAllocator()
{

}

void D3D12CommandListAllocator::Init(ID3D12Device* InDevice, UINT InAllocatorIndex)
{
	ThrowIfFailed(InDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator)));
	NAME_D3D12_OBJECT_WITHINDEX(CommandAllocator, InAllocatorIndex);
}