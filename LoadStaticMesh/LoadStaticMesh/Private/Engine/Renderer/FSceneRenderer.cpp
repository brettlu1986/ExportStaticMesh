
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

	//TODO: Create Scene Resource will move here
	GRHI->CreateSceneResources(RenderScene);
}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	GRHI->BeginRenderScene();

	GRHI->DrawSceneToShadowMap(RenderScene);

	GRHI->RenderSceneObjects(RenderScene);

	// draw skeletal mesh 
	{
		FUserMarker UserMarker("Draw Skeletal Mesh");
		
		std::vector<FResourceHeap*> Heaps; 
		FResourceViewCreater* ResViewCreater = GRHI->GetResourceViewCreater();
		Heaps.push_back(ResViewCreater->GetCbvSrvUavHeap());
		GRHI->SetResourceHeaps(Heaps);

		const std::vector<FSkeletalMesh*>& SkmMeshes = RenderScene->GetDrawSkeletalMeshes();
		for (size_t i = 0; i < SkmMeshes.size(); i++)
		{	
			FD3DGraphicPipline* Pso = GRHI->GetPsoObject(SkmMeshes[i]->GetPsoKey());
			GRHI->SetVertexAndIndexBuffers(SkmMeshes[i]->GetVertexBuffer(), SkmMeshes[i]->GetIndexBuffer());
			GRHI->SetPiplineStateObject(Pso);
			GRHI->SetResourceParams(0, SkmMeshes[i]->MtConstantBufferView);
			GRHI->SetResourceParams(2, RenderScene->PassContantView);
			GRHI->SetResourceParams(3, SkmMeshes[i]->ShaderResView);
			GRHI->DrawTriangleList(SkmMeshes[i]->GetIndexBuffer());
		}
	}
	//
	
	GRHI->EndRenderScene();
}

