
#include "stdafx.h"
#include "D3D12Fence.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Helper.h"

D3D12Fence::D3D12Fence(const std::string& Name, D3D12Adapter* InParent)
	:GenericFence(Name)
	,ParentAdapter(InParent)
	,FenceValue(0)
	,LastSignalFenceValue(0)
{
	CreateFence();
}

D3D12Fence::~D3D12Fence()
{
	
}

void D3D12Fence::ShutDown()
{
	Fence.Reset();
	CloseHandle(FenceEvent);
}

void D3D12Fence::CreateFence()
{
	ID3D12Device* Device = ParentAdapter->GetD3DDevice();
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	NAME_D3D12_OBJECT(Fence);
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void D3D12Fence::Initialize()
{

}

void D3D12Fence::SignalCurrentFence()
{
	D3D12Device* Device = ParentAdapter->GetDevice();
	ID3D12CommandQueue* CommandQueue = Device->GetD3DCommandQueue();

	UINT64 FenceNum = FenceValue;
	LastSignalFenceValue = FenceNum;
	ThrowIfFailed(CommandQueue->Signal(Fence.Get(), FenceNum));
	FenceValue ++;

	if(Fence->GetCompletedValue() < FenceNum)
	{
		ThrowIfFailed(Fence->SetEventOnCompletion(FenceNum, FenceEvent));
		WaitForSingleObject(FenceEvent, INFINITE);
	}
	
}

UINT64 D3D12Fence::GetCompletedValue()
{
	return Fence->GetCompletedValue();
}

bool D3D12Fence::IsFenceComplete()
{
	return GetCompletedValue() == LastSignalFenceValue;
}