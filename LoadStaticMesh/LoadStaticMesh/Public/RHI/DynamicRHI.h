#pragma once

#include "RHIDef.h"
#include "RHIResource.h"

struct RHIViewPort
{
public:
	RHIViewPort(float SizeX, float SizeY)
		:Width(SizeX)
		, Height(SizeY)
	{
	}
	float Width;
	float Height;
};

struct RHIScissorRect
{
public:
	RHIScissorRect(LONG InLeft, LONG InTop, LONG InRight, LONG InBottom)
		:Left(InLeft)
		, Top(InTop)
		, Right(InRight)
		, Bottom(InBottom)
	{
	}
	LONG Left;
	LONG Top;
	LONG Right;
	LONG Bottom;
};

struct RHISwapObjectInfo
{
	RHISwapObjectInfo(UINT InWidth, UINT InHeight, UINT InObjectCount, void* InWindow)
	:Width(InWidth)
	,Height(InHeight)
	,ObjectCount(InObjectCount)
	,WindowHandle(InWindow)
	{
	}
	
	UINT Width;
	UINT Height;
	UINT ObjectCount;
	void* WindowHandle;
};

class DynamicRHI
{
public:

	virtual ~DynamicRHI() { }

	virtual void Init() = 0;

	virtual void PostInit() {}

	virtual void ShutDown() = 0;

	virtual GenericFence* RHICreateFence(const std::string& Name) 
	{
		return new GenericFence(Name);
	}

	virtual void RHICreateViewPort(RHIViewPort& ViewPort) = 0;
	virtual void RHICreateSwapObject(RHISwapObjectInfo& SwapInfo) = 0;
};

extern DynamicRHI* GDynamicRHI;
extern EDynamicModuleType DynamicModuleType;

DynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:

	virtual bool IsSupported() = 0;
	virtual DynamicRHI* CreateRHI() = 0;

};