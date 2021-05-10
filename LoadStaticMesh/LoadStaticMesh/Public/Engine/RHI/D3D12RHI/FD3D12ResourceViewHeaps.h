#pragma once

#include "stdafx.h"
#include "FResourceViewHeaps.h"

class FD3D12ResourceViewHeap
{
public:
	void OnCreate(ID3D12Device* Device, D3D12_DESCRIPTOR_HEAP_TYPE HeapType, UINT DescriptorCount);
	void OnDestroy();
	bool AllocDescriptor(UINT Size, FResourceView* Rv);

};

class FD3D12ResourceViewHeaps : public FResourceViewHeaps
{
public:
	FD3D12ResourceViewHeaps();
	FD3D12ResourceViewHeaps(ID3D12Device* Device);
	virtual ~FD3D12ResourceViewHeaps();

	virtual void OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) override;
	virtual void OnDestroy() override;

private:
	ID3D12Device* ParentDevice;

	FD3D12ResourceViewHeap DsvHeap;
	FD3D12ResourceViewHeap RtvHeap;
	FD3D12ResourceViewHeap SamplerHeap;
	FD3D12ResourceViewHeap CbvSrvUavHeap;
};
