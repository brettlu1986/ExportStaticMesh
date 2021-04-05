#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"
#include "FDynamicRHI.h"
#include "FRHI.h"
#include "LAssetDataLoader.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::FSceneRenderer(UINT InWidth, UINT InHeight)
	:Width(InWidth)
	,Height(InHeight)
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderInit(FScene* Scene)
{
	//create adapter , create device, create command queue, create command manager, create command allocator/list
	
	/*FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_vs.cso");
	CreatePiplineStateObject(Vs, Ps);*/
	// load shader object
	UINT8* pVs = nullptr;
	UINT VsLen = 0;
	UINT8* pPs = nullptr;
	UINT PsLen = 0;
	GDynamicRHI->RHIReadShaderDataFromFile(LAssetDataLoader::GetAssetFullPath(L"shader_vs.cso"), &pVs, &VsLen);
	GDynamicRHI->RHIReadShaderDataFromFile(LAssetDataLoader::GetAssetFullPath(L"shader_ps.cso"), &pPs, &PsLen);
	FRHIInputElement RHIInputElementDescs[] =
	{
		{ "POSITION", 0, ERHI_DATA_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, ERHI_DATA_FORMAT::FORMAT_R32G32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	FRHIPiplineStateInitializer RHIPsoInitializer = {
		RHIInputElementDescs,
		_countof(RHIInputElementDescs),
		pVs,
		VsLen,
		pPs,
		PsLen,
		1
	};
	GDynamicRHI->RHICreatePiplineStateObject(RHIPsoInitializer);




	Scene->InitRenderResource();
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render(FRHICommandList& CommandList, FScene* Scene) 
{
	Scene->Render(CommandList);
}