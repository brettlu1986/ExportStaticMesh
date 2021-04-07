#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"
#include "FDynamicRHI.h"
#include "FRHI.h"
#include "LAssetDataLoader.h"
#include "FDefine.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderInit(FScene* Scene)
{
	Scene->InitRenderResource();

	UINT BufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	CreateConstantBuffer(BufferSize, sizeof(ObjectConstants));

	//IMPORTANT: after create pso, will excute first init command queue in the function
	FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_ps.cso");
	CreatePiplineStateObject(Vs, Ps, true);

	delete Vs;
	delete Ps;
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render( FScene* Scene) 
{
	Scene->Render();
}