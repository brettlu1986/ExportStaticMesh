
#include "stdafx.h"
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "LCamera.h"
#include "LSceneCamera.h"
#include "FShadowMap.h"

#include "LThirdPersonCamera.h"
#include "LPlayerController.h"

FScene::FScene()
:ConstantDirty(1)
,ShadowMap(nullptr)
{
	Init();
}

FScene::~FScene()
{
	Destroy();
}

void FScene::Init()
{
	ShadowMap = new FShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
}

void FScene::ActiveCamera(UINT CameraIndex)
{
	if(CameraIndex > Cameras.size() -1)
	{
		assert(!"camera index should not above camera size");
	}

	for(UINT i = 0 ; i < Cameras.size(); i++)
	{
		Cameras[i]->SetActive(CameraIndex == i);
	}
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

	std::vector<LCamera*>::iterator it = Cameras.begin();
	for(; it < Cameras.end(); it++)
	{
		LCamera* Ca = *it;
		delete Ca;
	}

	if(ShadowMap)
	{
		delete ShadowMap;
		ShadowMap = nullptr;
	}
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	Meshes.push_back(Mesh);
}

void FScene::AddCharacterToScene(LCharacter* Character)
{
	Players.push_back(Character);
	SkmMeshes.push_back(Character->GetSkeletalMesh());
}

void FScene::AddLightToScene(FLight* Light)
{
	SceneLights.push_back(Light);
	if(Light->LightIndex == 0)
	{
		Light->Init();
	}
}

void FScene::Update(float DeltaTime)
{
	GetActiveCamera()->Update(DeltaTime);

	for(LCharacter* Ch : Players)
	{
		Ch->Update(DeltaTime);
	}

	FLight* Light = GetLight(0);
	Light->Update(DeltaTime);
}


