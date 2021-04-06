#pragma once

#include "FRHIDef.h"
#include "FDefine.h"

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FShader.h"

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

	/// /////////////////////

	virtual void RHIInitWindow(UINT Width , UINT Height, void* Window) = 0;

	virtual void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize) = 0;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) = 0;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps) = 0;

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) = 0;
	virtual void RHIRenderEnd(UINT TargetFrame) = 0;
	virtual void RHIFirstPresent()= 0;

	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual FTexture* RHICreateTexture() = 0;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) = 0;
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

FORCEINLINE void RHIInitWindow(UINT Width, UINT Height, void* Window)
{
	GDynamicRHI->RHIInitWindow(Width, Height, Window);
}

FORCEINLINE void RHICreateConstantBuffer(UINT BufferSize, void* pDataFrom, UINT DataSize)
{
	return GDynamicRHI->RHICreateConstantBuffer(BufferSize, pDataFrom, DataSize);
}

FORCEINLINE void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize)
{
	return GDynamicRHI->RHIUpdateConstantBuffer(pUpdateData, DataSize);
}

FORCEINLINE  FShader* CreateShader(LPCWSTR ShaderFile)
{
	return GDynamicRHI->RHICreateShader(ShaderFile);
}

FORCEINLINE  void CreatePiplineStateObject(FShader* Vs, FShader* Ps) 
{
	GDynamicRHI->RHICreatePiplineStateObject(Vs, Ps);
}

FORCEINLINE void InitRenderBegin(UINT TargetFrame, FRHIColor Color)
{
	return GDynamicRHI->RHIInitRenderBegin(TargetFrame, Color);
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

FORCEINLINE FTexture* CreateTexture() 
{
	return GDynamicRHI->RHICreateTexture();
}

FORCEINLINE void InitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture)
{
	return GDynamicRHI->RHIInitMeshGPUResource(IndexBuffer, VertexBuffer, Texture);
}

FORCEINLINE void DrawMesh(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer)
{
	return GDynamicRHI->RHIDrawMesh(IndexBuffer, VertexBuffer);
}

FORCEINLINE void FirstPresent()
{
	return GDynamicRHI->RHIFirstPresent();
}