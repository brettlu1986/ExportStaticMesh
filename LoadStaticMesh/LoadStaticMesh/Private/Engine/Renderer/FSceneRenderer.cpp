
#include "stdafx.h"
#include "FSceneRenderer.h"
#include "FSkeletalMesh.h"
#include "LCharacter.h"

FSceneRenderer::FSceneRenderer()
{
}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::Initialize(FScene* RenderScene)
{
	//TODO: Create Scene Resource will move here
	GRHI->CreateSceneResources(RenderScene);

	const UINT CbvCount = 50;
	const UINT SrvCount = 100;
	const UINT UavCount = 10;
	const UINT DsvCount = 10;
	const UINT RtvCount = 60;
	const UINT SamplerCount = 20;
	GRHI->CreateResourceViewCreater(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);

	const std::vector<LCharacter*>& Players = RenderScene->GetCharacters();
	GRHI->CreatePipelineStateObject({"SKMPso", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), L"SkeletalVs.cso",
		L"SkeletalPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState()});

	for(LCharacter* Player : Players)
	{
		FSkeletalMesh* Mesh = Player->GetSkeletalMesh();
		Mesh->InitRenderResource();
		Mesh->SetPsoKey("SKMPso");
	}
}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	GRHI->BeginRenderScene();

	GRHI->DrawSceneToShadowMap(RenderScene);

	GRHI->RenderSceneObjects(RenderScene);


	
	GRHI->EndRenderScene();
}

