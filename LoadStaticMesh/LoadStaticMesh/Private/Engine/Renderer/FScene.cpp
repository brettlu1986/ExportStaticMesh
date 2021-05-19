
#include "stdafx.h"
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "LCamera.h"


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
	Character->PlayAnimation("Idle", true);
}

void FScene::AddLightToScene(FLight* Light)
{
	SceneLights.push_back(Light);
	if(Light->LightIndex == 0)
	{
		// assume that focus to {0, 0, 0}
		XMVECTOR Target = XMVector3Length(XMVectorSet(Light->Position.x, Light->Position.y, Light->Position.z, 1.f));
		Radius = XMVectorGetX(Target);
		Alpha = acosf(Light->Position.z / Radius);
		Theta = atanf(Light->Position.x / Light->Position.y);
		if (Theta < 0.f)
			Theta += XM_PI * 2;
		Alpha = MathHelper::Clamp(Alpha, 0.1f, MathHelper::Pi - 0.1f);
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
	if (Light)
	{
		Theta -= 0.001f;
		XMVECTOR V = DirectX::XMVectorSet(Radius * sinf(Alpha) * sinf(Theta),
			Radius * sinf(Alpha) * cosf(Theta),
			Radius * cosf(Alpha),
			1.f);
		Light->Position.x = XMVectorGetX(V);
		Light->Position.y = XMVectorGetY(V);
		Light->Position.z = XMVectorGetZ(V);

		XMVECTOR Direction = XMVectorSet(-Light->Position.x, -Light->Position.y, -Light->Position.z, 1.f);
		Direction = XMVector3Normalize(Direction);
		Light->Direction.x = XMVectorGetX(Direction);
		Light->Direction.y = XMVectorGetY(Direction);
		Light->Direction.z = XMVectorGetZ(Direction);

	}
}


