#pragma once

#include "FRHIDef.h"
#include "FDefine.h"

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FShader.h"
#include "FMesh.h"
#include "FScene.h"
#include "FD3D12Resource.h"
#include <map>

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

struct FFrameResource
{
	std::vector<FMesh*> MeshRenderResources;
	std::map<E_CONSTANT_BUFFER_TYPE, FD3DConstantBuffer*> ConstantBuffers;
	UINT NumFrameDirty = 1;
};

class FDynamicRHI
{
public:

	virtual ~FDynamicRHI() { }

	virtual void Init() = 0;

	virtual void ShutDown() = 0;

	virtual void RHIUpdateConstantBuffer(void* pUpdateData) = 0;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;

	virtual void RHIBeginRenderFrame(UINT TargetFrame) = 0;
	virtual void RHIPresentToScreen(UINT TargetFrame, bool bFirstExcute = false) = 0;

	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual FTexture* RHICreateTexture() = 0;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) = 0;
	virtual void RHIDrawMesh(FMesh* Mesh) = 0;

	virtual void RHICreateFrameResources(FScene* Scene) = 0;
	virtual void RHIUpdateFrameResources(FScene* Scene, UINT FrameIndex) = 0;
	virtual FFrameResource& RHIGetFrameResource(UINT FrameIndex) = 0;
	virtual void RHIRenderFrameResource(FFrameResource& FrameResource) = 0;
};

extern FDynamicRHI* GDynamicRHI;
extern EDynamicModuleType DynamicModuleType;

FDynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:
	virtual FDynamicRHI* CreateRHI() = 0;
};

FORCEINLINE void UpdateConstantBuffer(void* pUpdateData)
{
	return GDynamicRHI->RHIUpdateConstantBuffer(pUpdateData);
}

FORCEINLINE  FShader* CreateShader(LPCWSTR ShaderFile)
{
	return GDynamicRHI->RHICreateShader(ShaderFile);
}

FORCEINLINE void BeginRenderFrame(UINT TargetFrame)
{
	return GDynamicRHI->RHIBeginRenderFrame(TargetFrame);
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

FORCEINLINE void CreateFrameResources(FScene* Scene)
{
	return GDynamicRHI->RHICreateFrameResources(Scene);
}

FORCEINLINE void UpdateFrameResources(FScene* Scene, UINT FrameIndex)
{
	return GDynamicRHI->RHIUpdateFrameResources(Scene, FrameIndex);
}

FORCEINLINE FFrameResource& GetFrameResource(UINT FrameIndex) 
{
	return GDynamicRHI->RHIGetFrameResource(FrameIndex);
}

FORCEINLINE void RenderFrameResources(FFrameResource& FrameResource)
{
	GDynamicRHI->RHIRenderFrameResource(FrameResource);
}
