#pragma once

#include "FRHIDef.h"
#include "FDefine.h"

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FShader.h"
#include "FMesh.h"

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

	virtual void RHICreateSrvAndCbvs(FCbvSrvDesc) = 0;
	virtual void RHIUpdateConstantBuffer(void* pUpdateData) = 0;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;
	virtual void RHICreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey, bool bDefaultPso) = 0;

	virtual void RHIInitRenderBegin(UINT TargetFrame, FRHIColor Color) = 0;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) = 0;

	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual FTexture* RHICreateTexture() = 0;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) = 0;
	virtual void RHIDrawMesh(FMesh* Mesh) = 0;
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

FORCEINLINE void CreateSrvAndCbvs(FCbvSrvDesc Desc)
{
	return GDynamicRHI->RHICreateSrvAndCbvs(Desc);
}

FORCEINLINE void UpdateConstantBuffer(void* pUpdateData)
{
	return GDynamicRHI->RHIUpdateConstantBuffer(pUpdateData);
}

FORCEINLINE  FShader* CreateShader(LPCWSTR ShaderFile)
{
	return GDynamicRHI->RHICreateShader(ShaderFile);
}

FORCEINLINE  void CreatePiplineStateObject(FShader* Vs, FShader* Ps, const std::string& PsoKey, bool bDefaultPso = false)
{
	GDynamicRHI->RHICreatePiplineStateObject(Vs, Ps, PsoKey, bDefaultPso);
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

FORCEINLINE void DrawMesh(FMesh* Mesh)
{
	return GDynamicRHI->RHIDrawMesh(Mesh);
}
