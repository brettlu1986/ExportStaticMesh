#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"
#include "FDynamicRHI.h"
#include "FRHI.h"
#include "LAssetDataLoader.h"
#include "FDefine.h"
#include "SampleAssets.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderInit(FScene* Scene)
{
	UINT ObjectNum = Scene->GetDrawObjectsNum();
	UINT BufferSize = (sizeof(ObjectConstants) + 255) & ~255;
	BufferSize = BufferSize * ObjectNum;
	CreateConstantBuffer(BufferSize, ObjectNum);

	//IMPORTANT: after create pso, will excute first init command queue in the function
	FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_ps.cso");
	CreatePiplineStateObject(Vs, Ps, SampleAssets::PsoUseTexture, true);

	FShader* PsNoTex = CreateShader(L"shader_ps_notexture.cso");
	CreatePiplineStateObject(Vs, PsNoTex, SampleAssets::PsoNoTexture);

	delete Vs;
	delete Ps;
	delete PsNoTex;

	//in root sig, I put srv behind cbv, so Scene->InitRenderResource must after constant buffer create
	Scene->InitRenderResource();

	PresentToScreen(0, true);
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render( FScene* Scene) 
{
	Scene->Render();
}