#pragma once

#include "FRHIDef.h"
#include "FRHIResource.h"
#include "FDefine.h"

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"



struct FRHIViewPort
{
public:
	FRHIViewPort(float SizeX, float SizeY)
		:Width(SizeX)
		, Height(SizeY)
	{
	}
	float Width;
	float Height;
};

struct FRHIScissorRect
{
public:
	FRHIScissorRect(LONG InLeft, LONG InTop, LONG InRight, LONG InBottom)
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

struct FRHISwapObjectInfo
{
	FRHISwapObjectInfo(UINT InWidth, UINT InHeight, UINT InObjectCount, void* InWindow)
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

typedef enum class ERHI_DATA_FORMAT
{
	FORMAT_UNKNONWN = 0,
    FORMAT_R32G32B32_FLOAT,
	FORMAT_R32G32_FLOAT,
}ERHI_DATA_FORMAT;

typedef enum class ERHI_INPUT_CLASSIFICATION
{
	INPUT_CLASSIFICATION_PER_VERTEX_DATA = 0,
	INPUT_CLASSIFICATION_PER_INSTANCE_DATA = 1
} 	ERHI_INPUT_CLASSIFICATION;

struct FRHIInputElement
{
	std::string SemanticName;
	UINT SemanticIndex;
	ERHI_DATA_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	ERHI_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
};

//later will add more param, temp use these for this project
struct FRHIPiplineStateInitializer
{
    const FRHIInputElement* pInpueElement;
	UINT NumElements;
	BYTE* pVSPointer;
	SIZE_T VsPointerLength;
	BYTE* pPsPointer;
	SIZE_T PsPointerLength;
	UINT NumRenderTargets;
};

class FDynamicRHI
{
public:

	virtual ~FDynamicRHI() { }

	virtual void Init() = 0;

	virtual void ShutDown() = 0;

	virtual FGenericFence* RHICreateFence(const std::string& Name) 
	{
		return new FGenericFence(Name);
	}

	virtual void RHICreateViewPort(FRHIViewPort& ViewPort) = 0;
	virtual void RHICreateSwapObject(FRHISwapObjectInfo& SwapInfo) = 0;
	virtual void RHICreatePiplineStateObject(FRHIPiplineStateInitializer& Initializer) = 0;
	virtual void RHIReadShaderDataFromFile(std::wstring FileName, byte** Data, UINT* Size) = 0;
	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize) = 0;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) = 0;
	virtual FRHIView* RHICreateShaderResourceView(std::wstring TextureName) = 0;
	virtual FRHICommandList& RHIGetCommandList(UINT Index) = 0;
	virtual void RHIExcuteCommandList(FRHICommandList& CommandList) = 0;
	virtual void RHICloseCommandList(FRHICommandList& CommandList) = 0;
	virtual void RHISignalCurrentFence() = 0;
	virtual bool RHIIsFenceComplete() = 0;
	virtual void RHIResetCommandList(FRHICommandList& CommandList) = 0;
	virtual void RHISetCurrentViewPortAndScissorRect(FRHICommandList& CommandList) = 0;

	virtual void RHISetGraphicRootDescripterTable(FRHICommandList& CommandList) = 0;
	virtual void RHISwapObjectPresent() = 0;


	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) = 0;
	virtual void RHICreateRenderTarget(UINT Width, UINT Height) = 0;
	virtual void RHIRenderEnd(UINT TargetFrame) = 0;


	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer) = 0;
	virtual void RHIDrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer) = 0;
};


extern FDynamicRHI* GDynamicRHI;
extern EDynamicModuleType DynamicModuleType;

FDynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:

	virtual bool IsSupported() = 0;
	virtual FDynamicRHI* CreateRHI() = 0;

};

FORCEINLINE void InitRenderBegin(UINT TargetFrame, FRHIColor Color)
{
	return GDynamicRHI->RHIInitRenderBegin(TargetFrame, Color);
}

FORCEINLINE void CreateRenderTarget(UINT Width, UINT Height)
{
	GDynamicRHI->RHICreateRenderTarget(Width, Height);
}

FORCEINLINE void RenderEnd(UINT TargetFrame)
{
	GDynamicRHI->RHIRenderEnd(TargetFrame);
}

FORCEINLINE FIndexBuffer* CreateIndexBuffer()
{
	return GDynamicRHI->RHICreateIndexBuffer();
}

FORCEINLINE FVertexBuffer* CreateVertexBuffer()
{
	return GDynamicRHI->RHICreateVertexBuffer();
}

FORCEINLINE void InitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	return GDynamicRHI->RHIInitMeshGPUResource(IndexBuffer, VertexBuffer);
}

FORCEINLINE void DrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	return GDynamicRHI->RHIDrawMesh(IndexBuffer, VertexBuffer);
}