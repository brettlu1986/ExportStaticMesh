#pragma once

#include "stdafx.h"

class FGenericFence
{
public: 
	FGenericFence(const std::string& Name)
	:FenceName(Name)
	{}

	virtual void CreateFence() {};
	virtual void SignalCurrentFence() {};
	virtual UINT64 GetCompletedValue() { return 0; };
	virtual bool IsFenceComplete() { return false;};

protected:
	std::string FenceName;
};

class FRHIResource
{
public:
	FRHIResource() { InitData = nullptr; };
	FRHIResource(const std::string& InResourceName, void* InData)
	:ResourceName(InResourceName)
	,InitData(InData)
	{}
	virtual ~FRHIResource() {};

protected:
	void* InitData;
	std::string ResourceName;
};


class FRHIView
{
public:
	FRHIView()
	:Location(0)
	,StrideInBytes(0)
	,SizeInBytes(0)
	{  };
	FRHIView(UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
	:Location(InLocation)
	, StrideInBytes(InBytes)
	, SizeInBytes(InSizeBytes)
	{
	}
	virtual ~FRHIView() {};

	virtual void Clear() {};

protected:
	UINT64 Location;
	UINT StrideInBytes;
	UINT SizeInBytes;
};