#pragma once

#include "stdafx.h"

class GenericFence
{
public: 
	GenericFence(const std::string& Name)
	:FenceName(Name)
	{}

	virtual void CreateFence() {};
	virtual void SignalCurrentFence() {};
	virtual UINT64 GetCompletedValue() { return 0; };
	virtual bool IsFenceComplete() { return false;};

protected:
	std::string FenceName;
};

class RHIResource
{
public:
	RHIResource() { InitData = nullptr; };
	RHIResource(const std::string& InResourceName, void* InData)
	:ResourceName(InResourceName)
	,InitData(InData)
	{}
	virtual ~RHIResource() {};

protected:
	void* InitData;
	std::string ResourceName;
};


class RHIView
{
public:
	RHIView()
	:Location(0)
	,StrideInBytes(0)
	,SizeInBytes(0)
	{  };
	RHIView(UINT64 InLocation, UINT InBytes, UINT InSizeBytes)
	:Location(InLocation)
	, StrideInBytes(InBytes)
	, SizeInBytes(InSizeBytes)
	{
	}
	virtual ~RHIView() {};

	virtual void Clear() {};

protected:
	UINT64 Location;
	UINT StrideInBytes;
	UINT SizeInBytes;
};