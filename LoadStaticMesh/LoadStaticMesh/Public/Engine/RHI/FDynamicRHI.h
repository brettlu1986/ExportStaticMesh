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

class FDynamicRHI
{
public:

	virtual ~FDynamicRHI() { }

	virtual void Init() = 0;

	virtual void ShutDown() = 0;

	

	virtual void UpdateSceneResources(FScene* RenderScene) = 0;
	virtual void BeginRenderScene() = 0;
	virtual void EndRenderScene() = 0;
	virtual void DrawMesh(FMesh* Mesh, FD3DGraphicPipline* Pso) = 0;
	
	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual FTexture* RHICreateTexture() = 0;
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;
	virtual void RHIInitMeshGPUResource(FIndexBuffer* IndexBuffer, FVertexBuffer* VertexBuffer, FTexture* Texture) = 0;

	virtual void BeginCreateSceneResource() = 0;
	virtual void CreateSceneResources(FScene* Scene) = 0;
	virtual void RenderSceneObjects(FScene* Scene) = 0;
	virtual void EndCreateSceneResource() = 0;

	virtual void DrawSceneToShaderMap(FScene* Scene) = 0;
};

extern FDynamicRHI* GRHI;
extern EDynamicModuleType DynamicModuleType;

FDynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:
	virtual FDynamicRHI* CreateRHI() = 0;
};

