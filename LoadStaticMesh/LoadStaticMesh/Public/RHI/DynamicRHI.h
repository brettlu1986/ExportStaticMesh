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

typedef enum RHI_DATA_FORMAT
{
	FORMAT_UNKNONWN = 0,
    FORMAT_R32G32B32_FLOAT,
	FORMAT_R32G32_FLOAT,
}RHI_DATA_FORMAT;

typedef enum RHI_INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
} 	RHI_INPUT_CLASSIFICATION;

struct RHIInputElement
{
	std::string SemanticName;
	UINT SemanticIndex;
	RHI_DATA_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	RHI_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};

//later will add more param, temp use these for this project
struct RHIPiplineStateInitializer
{
    const RHIInputElement* pInpueElement;
	UINT NumElements;
	BYTE* pVSPointer;
	SIZE_T VsPointerLength;
	BYTE* pPsPointer;
	SIZE_T PsPointerLength;
	UINT NumRenderTargets;
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
	virtual void RHICreatePiplineStateObject(RHIPiplineStateInitializer& Initializer) = 0;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) = 0;
	virtual void RHICreateRenderTarget(UINT TargetCount) = 0;
	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, void **pDataMap) = 0;
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