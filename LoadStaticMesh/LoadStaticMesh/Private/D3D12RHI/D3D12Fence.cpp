
#include "stdafx.h"
#include "D3D12Fence.h"
#include "D3D12Adapter.h"
#include "D3D12Helper.h"

D3D12Fence::D3D12Fence(const std::string& Name, D3D12Adapter* InParent)
	:GenericFence(Name)
	,ParentAdapter(InParent)
	,FenceValue(0)
{
	CreateFence();
}

D3D12Fence::~D3D12Fence()
{

}

void D3D12Fence::CreateFence()
{
	ID3D12Device* Device = ParentAdapter->GetRootDevice();
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	NAME_D3D12_OBJECT(Fence);
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void D3D12Fence::SignalCurrentFence()
{
	UINT64 FenceNum = FenceValue;

}

UINT64 D3D12Fence::GetCompletedValue()
{
	return 0;
}

bool D3D12Fence::IsFenceComplete()
{
	return true;

}