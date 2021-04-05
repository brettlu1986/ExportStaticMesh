
#include "stdafx.h"
#include "FD3D12Fence.h"
#include "FD3D12Adapter.h"
#include "FD3D12Helper.h"

FD3D12Fence::FD3D12Fence(const std::string& Name, FD3D12Adapter* InParent)
	:FGenericFence(Name)
	,ParentAdapter(InParent)
	,FenceValue(0)
	,LastSignalFenceValue(0)
{
	CreateFence();
}

FD3D12Fence::~FD3D12Fence()
{
	
}

void FD3D12Fence::ShutDown()
{
	Fence.Reset();
	CloseHandle(FenceEvent);
}

void FD3D12Fence::CreateFence()
{
	ID3D12Device* Device = ParentAdapter->GetD3DDevice();
	ThrowIfFailed(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)));
	NAME_D3D12_OBJECT(Fence);
	FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void FD3D12Fence::Initialize()
{

}

void FD3D12Fence::SignalCurrentFence()
{
	ID3D12CommandQueue* CommandQueue = ParentAdapter->GetD3DCommandQueue();

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

UINT64 FD3D12Fence::GetCompletedValue()
{
	return Fence->GetCompletedValue();
}

bool FD3D12Fence::IsFenceComplete()
{
	return GetCompletedValue() == LastSignalFenceValue;
}