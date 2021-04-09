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

class FDynamicRHI
{
public:

	virtual ~FDynamicRHI() { }

	virtual void Init() = 0;

	virtual void ShutDown() = 0;

	virtual void RHICreateConstantBuffer(UINT BufferSize, UINT DataSize) = 0;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData, UINT DataSize) = 0;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps) = 0;

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) = 0;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) = 0;

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

FORCEINLINE void CreateConstantBuffer(UINT BufferSize, UINT DataSize)
{
	return GDynamicRHI->RHICreateConstantBuffer(BufferSize, DataSize);
}

FORCEINLINE void UpdateConstantBuffer(void* pUpdateData, UINT DataSize)
{
	return GDynamicRHI->RHIUpdateConstantBuffer(pUpdateData, DataSize);
}

FORCEINLINE  FShader* CreateShader(LPCWSTR ShaderFile)
{
	return GDynamicRHI->RHICreateShader(ShaderFile);
}

FORCEINLINE  void CreatePiplineStateObject(FShader* Vs, FShader* Ps, bool bExcute = false)
{
	GDynamicRHI->RHICreatePiplineStateObject(Vs, Ps);
}

FORCEINLINE void InitRenderBegin(UINT TargetFrame, FRHIColor Color)
{
	return GDynamicRHI->RHIInitRenderBegin(TargetFrame, Color);
}

FORCEINLINE void PresentToScreen(UINT TargetFrame, bool bFirstExcute = false)
{
	GDynamicRHI->RHIPresentToScreen(TargetFrame, bFirstExcute);
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
