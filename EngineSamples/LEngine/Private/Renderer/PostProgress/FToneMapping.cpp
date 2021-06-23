
#include "pch.h"
#include "FToneMapping.h"
#include "FPostResources.h"
#include "FRHI.h"

FToneMapping::FToneMapping()
	:ToneMapTex(nullptr)
	, RTVToneMap(nullptr)
	, SRVToneMap(nullptr)
{

}

FToneMapping::~FToneMapping()
{
	//tone map
	SAFE_DESTROY(ToneMapTex);
	SAFE_DELETE(RTVToneMap);
	SAFE_DELETE(SRVToneMap);
}

void FToneMapping::Initialize(FPostResources* PostResIns)
{
	PostRes = PostResIns;

	//Tone map
	GRHI->CreatePipelineStateObject({ "ToneMap", FullInputElementDescs, _countof(FullInputElementDescs), "ToneMapVs",
		"ToneMapPs", 1, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });

	FClearColor ToneMapClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	FClearValue ToneMapClearValue;
	ToneMapClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM;
	ToneMapClearValue.ClearDepth = nullptr;
	ToneMapClearValue.ClearColor = &ToneMapClearColor;

	FTextureInitializer ToneMapTexInitializer =
	{
		(UINT)GRHI->GetDefaultViewPort().Width, (UINT)GRHI->GetDefaultViewPort().Height, 1, 1, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &ToneMapClearValue,
		E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
	};

	ToneMapTex = GRHI->CreateTexture(ToneMapTexInitializer);
	SRVToneMap = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &ToneMapTex,
		0, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM });
	RTVToneMap = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &ToneMapTex, 0, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM });

}

void FToneMapping::Render(FIndexBuffer* IndexBuffer)
{
	USER_MARKER("Tone Map");
	GRHI->SetViewPortInfo(GRHI->GetDefaultViewPort());
	GRHI->SetPiplineStateObject(GRHI->GetPsoObject("ToneMap"));
	
	//if there has other post progress, then use RTVToneMap
	//	GRHI->ResourceTransition(SRVToneMap, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
	//	GRHI->SetRenderTargets(RTVToneMap, nullptr);
	GRHI->ResourceTransition(PostRes->SRVSceneColor, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GRHI->ResourceTransition(PostRes->GetSunMerge()->GetSRV(), E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	GRHI->SetResourceParams(4, PostRes->SRVSceneColor);
	GRHI->SetResourceParams(5, PostRes->GetSunMerge()->GetSRV());
	GRHI->DrawTriangleList(IndexBuffer);
}

