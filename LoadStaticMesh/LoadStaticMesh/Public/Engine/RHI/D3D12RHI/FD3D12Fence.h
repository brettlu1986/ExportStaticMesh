#pragma once

#include "stdafx.h"
#include "FRHIResource.h"

using namespace Microsoft::WRL;

class FD3D12Adapter;
class FD3D12Fence : public FGenericFence
{
public:
	FD3D12Fence(const std::string& Name, FD3D12Adapter* InParent);
	virtual ~FD3D12Fence();
	void ShutDown();
	void Initialize();

	virtual void CreateFence() override;
	virtual void SignalCurrentFence() override;
	virtual UINT64 GetCompletedValue() override;
	virtual bool IsFenceComplete() override;
	
private:
	FD3D12Adapter* ParentAdapter;
	ComPtr<ID3D12Fence> Fence;
	HANDLE FenceEvent;
	UINT64 FenceValue;
	UINT64 LastSignalFenceValue;
};