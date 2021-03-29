#pragma once

class DynamicRHI
{
public:

	virtual ~DynamicRHI() { }

	virtual void Init() = 0;

	virtual void PostInit() {}

	virtual void ShutDown() = 0;

};

extern DynamicRHI* GDynamicRHI;

DynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:

	virtual bool IsSupported() = 0;
	virtual DynamicRHI* CreateRHI() = 0;
};