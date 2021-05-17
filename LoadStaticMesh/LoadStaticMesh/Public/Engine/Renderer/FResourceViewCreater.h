#pragma once

#include "stdafx.h"

class FResourceView
{
public:
	FResourceView() {};
	virtual ~FResourceView() = default;
};

class FResourceHeap
{
public: 
	FResourceHeap() {}; 
	virtual ~FResourceHeap() = default;

	virtual void OnCreate(ID3D12Device* Device, UINT HeapType, UINT InDescriptorCount) = 0;
	virtual void OnDestroy() = 0;
	virtual bool AllocDescriptor(UINT Count, FResourceView* Rv) = 0;
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