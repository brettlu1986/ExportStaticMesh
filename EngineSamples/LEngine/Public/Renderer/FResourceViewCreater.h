#pragma once

#include "pch.h"
#include "FDefine.h"
class FResourceView
{
public:
	FResourceView()
	:ResourceState(E_RESOURCE_STATE::RESOURCE_STATE_COMMON)
	{};
	virtual ~FResourceView() = default;

	void SetResourceState(E_RESOURCE_STATE State) 
	{
		ResourceState = State;
	}

	E_RESOURCE_STATE GetResourceState() 
	{
		return ResourceState;
	}

protected:
	
	E_RESOURCE_STATE ResourceState;
};

class FResourceHeap
{
public: 
	FResourceHeap() {}; 
	virtual ~FResourceHeap() = default;

	virtual void OnCreate(ID3D12Device* Device, UINT HeapType, UINT InDescriptorCount) = 0;
	virtual void OnDestroy() = 0;
	virtual bool AllocDescriptor(UINT Count, FResourceView* Rv, E_RESOURCE_VIEW_TYPE ViewType) = 0;
};

class FResourceViewCreater
{
public:
	FResourceViewCreater() 
	:DsvHeap(nullptr)
	,RtvHeap(nullptr)
	,SamplerHeap(nullptr)
	,CbvSrvUavHeap(nullptr)
	{};
	virtual ~FResourceViewCreater() = default;

	virtual void OnCreate(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) = 0;
	virtual void OnDestroy() = 0;

	FResourceHeap* GetDsvHeap() { return DsvHeap;}
	FResourceHeap* GetRtvHeap() { return RtvHeap; }
	FResourceHeap* GetSamplerHeap() { return SamplerHeap; }
	FResourceHeap* GetCbvSrvUavHeap() { return CbvSrvUavHeap; }

protected: 
	
	FResourceHeap* DsvHeap;
	FResourceHeap* RtvHeap;
	FResourceHeap* SamplerHeap;
	FResourceHeap* CbvSrvUavHeap;
};