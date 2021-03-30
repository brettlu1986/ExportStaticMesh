#pragma once

#include "RHIDef.h"
#include "RHIResource.h"

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