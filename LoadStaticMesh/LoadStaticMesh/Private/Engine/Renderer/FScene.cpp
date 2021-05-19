
#include "stdafx.h"
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "LCamera.h"
#include "LPlayerController.h"

FScene::FScene()
:MeshCount(0)
,MeshWithTextureCount(0)
,ConstantDirty(1)
{

}

FScene::~FScene()
{
	Destroy();
}

void FScene::Destroy()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Destroy();
	}
	Meshes.clear();

	delete PassContantView;
	PassContantView = nullptr;

	for (size_t i = 0; i < Players.size(); ++i)
	{
		Players[i]->Destroy();
	}
	Players.clear();
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	Mesh->SetMatrixCbIndex(MeshCount);
	Mesh->SetMaterialCbvHeapIndex(MeshCount);
	if(Mesh->GetDiffuseTexture())
	{
		Mesh->SetDiffuseTextureHeapIndex(MeshWithTextureCount);
		MeshWithTextureCount ++;
	}
	Meshes.push_back(Mesh);
	MeshCount++;
}

void FScene::AddCharacterToScene(LCharacter* Character)
{
	Players.push_back(Character);
	SkmMeshes.push_back(Character->GetSkeletalMesh());
}

void FScene::InitCharacters()
{
	if(Players.size() < 3)
	{
		assert(!"add more characters");
	}

	LPlayerController* PlayerController = new LPlayerController();
	PlayerController->Possess(Players[0]);

	Players[0]->SetPlayerController(PlayerController);
	Players[0]->PlayAnimation("Idle", true);

	Players[1]->PlayAnimation("Walk", true);
	Players[2]->PlayAnimation("Run", true);
}

void FScene::AddLightToScene(FLight* Light)
{
	SceneLights.push_back(Light);
	if(Light->LightIndex == 0)
	{
		Light->Init();
	}
}

void FScene::InitSceneRenderResource()
{
	for(size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->InitRenderResource();
		Meshes[i]->SetPsoKey(Meshes[i]->GetDiffuseTexture() != nullptr ? "PsoUseTexture" : "PsoNoTexture");
	}

	//TODO:move to Scene Render Init
	PassContantView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassConstants)) });
}

void FScene::Update(float DeltaTime)
{
	Camera.Update(DeltaTime);

	for(LCharacter* Ch : Players)
	{
		Ch->Update(DeltaTime);
	}

	FLight* Light = GetLight(0);
	Light->Update(DeltaTime);
}


