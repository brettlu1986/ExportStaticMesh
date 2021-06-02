#pragma once

#include "FRHIDef.h"
#include "FDefine.h"

#include "FVertexBuffer.h"
#include "FIndexBuffer.h"
#include "FTexture.h"
#include "FShader.h"
#include "FMesh.h"
#include "FSkeletalMesh.h"
#include "FScene.h"
#include "FD3D12Resource.h"
#include <map>
#include "FResourceViewCreater.h"


struct FRHIViewPort
{
public:
	FRHIViewPort(float InLeft, float InTop, float SizeX, float SizeY)
		:Left(InLeft)
		,Top(InTop)
		,Width(SizeX)
		,Height(SizeY)
	{
	}
	float Left;
	float Top;
	float Width;
	float Height;
};

struct FResourceViewInfo
{
	E_RESOURCE_VIEW_TYPE ViewType;
	UINT ViewCount;
	FTexture** TexResource;// this is an array pointer, for one just use the tex pointer address
	UINT DataSize;//use for cbv
	E_GRAPHIC_FORMAT Format;//if the texture has bitdata, the format will use the ResourceLoad format, like dds read
	UINT Index = 0;// use for swap chain rtv index.  TODO:think another way to do this
};

class FDynamicRHI
{
public:

	virtual ~FDynamicRHI() { }

	virtual void Init() = 0;
	virtual void ShutDown() = 0;
	
	virtual void BeginRenderScene() = 0;
	//virtual void UpdateSceneResources(FScene* RenderScene) = 0;
	virtual void EndRenderScene() = 0;

	//virtual void BeginCreateSceneResource() = 0;
	//virtual void EndCreateSceneResource() = 0;

	virtual FIndexBuffer* RHICreateIndexBuffer() = 0;
	virtual FVertexBuffer* RHICreateVertexBuffer() = 0;
	virtual void UpdateVertexBufferResource(FVertexBuffer* VertexBuffer, LVertexBuffer& VertexBufferData) = 0;
	virtual void UpdateIndexBufferResource(FIndexBuffer* IndexBuffer, LIndexBuffer& IndexBufferData) = 0;
	
	virtual FShader* RHICreateShader(LPCWSTR ShaderFile) = 0;
	virtual FTexture* RHICreateTexture() = 0;
	virtual FTexture* CreateTexture(FTextureInitializer TexInitializer) = 0;
	
	virtual void CreateResourceViewCreater(UINT CbvCount, UINT SrvCount, UINT UavCount, UINT DsvCount, UINT RtvCount, UINT SamplerCount) = 0;
	//the Texuture**  is the texture array start pointer, it can create and bind more than one texture onece
	virtual FResourceView* CreateResourceView(FResourceViewInfo ViewInfo) = 0;
	virtual void UpdateConstantBufferView(FResourceView* CbvView, void* pDataUpdate) = 0;
	virtual void CreatePipelineStateObject(FPiplineStateInitializer Initializer) = 0;

	virtual void SetResourceHeaps(vector<FResourceHeap*>& Heaps ) = 0;
	virtual void SetRenderTargets(FResourceView* RtvView, FResourceView* DsvView) = 0;
	virtual void SetVertexAndIndexBuffers(FVertexBuffer* VertexBuffer, FIndexBuffer* IndexBuffer) = 0;
	virtual void SetPiplineStateObject(FD3DGraphicPipline* PsoObj) = 0;
	virtual void SetResourceParams(UINT Index, FResourceView* ResView) = 0;
	virtual void DrawTriangleList(FIndexBuffer* IndexBuffer) = 0;
	virtual void ResourceTransition(FResourceView* ResourceView, E_RESOURCE_STATE StateFrom, E_RESOURCE_STATE StateTo) = 0;

	virtual void BeginEvent(const char* Name) = 0;
	virtual void EndEvent() = 0;

	virtual UINT GetFrameIndex() = 0;
	virtual void SetViewPortInfo(FRHIViewPort ViewPort) = 0;
	virtual FRHIViewPort GetDefaultViewPort() = 0;
public: 
	FResourceViewCreater* GetResourceViewCreater()
	{
		return ResourceViewCreater;
	}
	FD3DGraphicPipline* GetPsoObject(string KeyName)
	{
		return PiplelineStateObjCache[KeyName];
	}

protected:
	FResourceViewCreater* ResourceViewCreater = nullptr;
	map<string, FD3DGraphicPipline*> PiplelineStateObjCache;

};

extern FDynamicRHI* GRHI;
extern EDynamicModuleType DynamicModuleType;

FDynamicRHI* PlatformCreateDynamicRHI();

class IDynamicRHIModule
{
public:
	virtual FDynamicRHI* CreateRHI() = 0;
};


class FUserMarker
{
public:
	FUserMarker(const char* Name)
	{
		GRHI->BeginEvent(Name);
	}
	~FUserMarker()
	{
		GRHI->EndEvent();
	}
};
