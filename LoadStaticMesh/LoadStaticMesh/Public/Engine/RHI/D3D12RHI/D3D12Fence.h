#pragma once

#include "stdafx.h"
#include "RHIResource.h"

using namespace Microsoft::WRL;

class D3D12Adapter;
class D3D12Fence : public GenericFence
{
public:
	D3D12Fence(const std::string& Name, D3D12Adapter* InParent);
	virtual ~D3D12Fence();
	void ShutDown();
	void Initialize();

	virtual void CreateFence() override;
	virtual void SignalCurrentFence() override;
	virtual UINT64 GetCompletedValue() override;
	virtual bool IsFenceComplete() override;
	
private:
	D3D12Adapter* ParentAdapter;
	ComPtr<ID3D12Fence> Fence;
	HANDLE FenceEvent;
	UINT64 FenceValue;
	UINT64 LastSignalFenceValue;
};