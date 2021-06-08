
#include "stdafx.h"
#include "SampleAssets.h"

#include "LAssetDataLoader.h"
#include "LSceneCamera.h"
#include "LThirdPersonCamera.h"
#include "LPlayerController.h"
#include "LLight.h"
#include "LSkeletalMesh.h"
#include "LAnimator.h"
#include "LScene.h"
#include "LCharacter.h"
#include "LAssetManager.h"

void SampleAssets::LoadSampleSceneData(LScene& Scene)
{
	//load assets
	LAssetManager::Get()->LoadSkeletal(SampleAssets::SkeletonResource.FileName, SampleAssets::SkeletonResource.RefName);

	for (UINT i = 0; i < SampleAssets::ShaderResCount; i++)
	{
		LAssetManager::Get()->LoadShader(SampleAssets::ShaderResources[i].FileName, SampleAssets::ShaderResources[i].RefName);
	}

	for (UINT i = 0; i < SampleAssets::DDSTexResourcesCount; i++)
	{
		LAssetManager::Get()->LoadTexture(SampleAssets::DDSTexResources[i].FileName, SampleAssets::DDSTexResources[i].RefName);
	}

	auto DefaultMat = make_unique<LMaterial>();
	LAssetManager::Get()->LoadMaterial(DefaultMat, "DefaultMat");

	auto ChairMat = make_unique<LMaterial>();
	ChairMat->SetColorTex(LAssetManager::Get()->GetTexture("ChairColor"));
	LAssetManager::Get()->LoadMaterial(ChairMat, "ChairMat");

	auto CharacterMat = make_unique<LMaterial>();
	CharacterMat->SetColorTex(LAssetManager::Get()->GetTexture("MaleColor"));
	LAssetManager::Get()->LoadMaterial(CharacterMat, "CharacterMat");
	
	//load static mesh
	for (UINT i = 0; i < SampleAssets::SamepleCount; i++)
	{
		XMFLOAT3 Location, Rotation, Scale;
		auto Mesh = make_shared<LMesh>();
		Mesh->SetMaterial(LAssetManager::Get()->GetMaterial(i == 0 ? "ChairMat" : "DefaultMat"));
		LAssetDataLoader::LoadMeshFromFile(SampleAssets::SampleResources[i], *Mesh, Location, Rotation, Scale);
		Mesh->InitRenderThreadResource();

		Mesh->SetModelLocation(Location);
		Mesh->SetModelRotation(Rotation);
		Mesh->SetModelScale(Scale);

		Scene.AddStaticMeshes(Mesh);
	}

	vector<DirectionLightData> DirectionLights;
	LAssetDataLoader::LoadDirectionLights(SampleAssets::SceneLightsFile, DirectionLights);
	for (size_t i = 0; i < DirectionLights.size(); i++)
	{
		auto Light = make_shared<LLight>();
		Light->Direction = DirectionLights[i].Direction;
		Light->Position = DirectionLights[i].Position;
		Light->Strength = DirectionLights[i].Strength;
		Light->LightIndex = (UINT)i;
		//only change the first light param
		Light->Init();
		Light->InitRenderThreadResource();
		Scene.AddLightToScene(Light);
	}

	for (UINT i = 0; i < SampleAssets::SampleSkeletalMeshCount; i++)
	{
		XMFLOAT3 Location, Rotation, Scale;
		//character has skeleton mesh and animator instance
		auto Character = make_shared<LCharacter>();

		//create skeletal mesh, skeleton save in skeletal mesh
		LSkeletalMesh* SkeletalMesh = new LSkeletalMesh();
		SkeletalMesh->SetMaterial(LAssetManager::Get()->GetMaterial("CharacterMat"));
		LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(SampleAssets::SkeletalMeshResource[i], *SkeletalMesh, Location, Rotation, Scale);
		SkeletalMesh->InitRenderThreadResource();

		SkeletalMesh->SetModelLocation(Location);
		SkeletalMesh->SetModelRotation(Rotation);
		SkeletalMesh->SetModelScale(Scale);

		SkeletalMesh->SetSkeleton(LAssetManager::Get()->GetSkeletal(SampleAssets::SkeletonResource.RefName));

		//create animator
		LAnimator* Animator = new LAnimator();
		for (UINT i = 0; i < (UINT)E_ANIM_STATE::STATE_COUNT; i++)
		{
			LAnimationSequence AnimSequence;
			LAssetDataLoader::LoadAnimationSquence(SampleAssets::SkeletalAnim[i], AnimSequence);
			Animator->AddAnimSequence(static_cast<E_ANIM_STATE>(i), AnimSequence);
		}

		//set skeletal mesh and animator to character
		Character->SetSkeletalMesh(SkeletalMesh);
		Character->SetAnimator(Animator);

		//add player to scene
		Scene.AddCharacterToScene(Character);
	}

	//load scene camera
	shared_ptr<LCamera> SceneCamera = make_shared<LSceneCamera>();
	LAssetDataLoader::LoadCameraDataFromFile(SampleAssets::CameraBin, *SceneCamera);
	Scene.AddCamera(SceneCamera);

	shared_ptr<LCamera> ThirdPersonCamera = make_shared<LThirdPersonCamera>();
	LAssetDataLoader::LoadCameraDataFromFile(SampleAssets::CameraBin, *ThirdPersonCamera);
	ThirdPersonCamera->SetSocketOffset(XMFLOAT3(-2.f, 0.3f, 2.2f));
	ThirdPersonCamera->SetViewTarget(Scene.GetCharacters()[0].get());
	Scene.AddCamera(ThirdPersonCamera);

	//set the first character is the player
	LPlayerController* PlayerController = new LPlayerController();
	PlayerController->Possess(Scene.GetCharacters()[0].get());

	Scene.ActiveCamera(0);
}

