#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"
#include "FDynamicRHI.h"
#include "FRHI.h"
#include "LAssetDataLoader.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderInit(FScene* Scene)
{
	//create adapter , create device, create command queue, create command manager, create command allocator/list
	FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_ps.cso");
	CreatePiplineStateObject(Vs, Ps);

	delete Vs;
	delete Ps;
	Scene->InitRenderResource();
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render( FScene* Scene) 
{
	Scene->Render();
}