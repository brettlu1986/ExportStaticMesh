
#include "stdafx.h"
#include "SampleAssets.h"

#include "LAssetDataLoader.h"

#include "FRHI.h"
#include "FMesh.h"

#include "LSceneCamera.h"
#include "LThirdPersonCamera.h"
#include "LPlayerController.h"

vector<LSkeleton*> ResourceSkeletons;

void SampleAssets::ReleaseAssets()
{
	vector<LSkeleton*>::iterator it = ResourceSkeletons.begin();
	for (; it < ResourceSkeletons.end(); it++)
	{
		LSkeleton* Sk = *it;
		delete Sk;
	}


}

void SampleAssets::LoadSampleScene(FScene* Scene)
{
	for (UINT i = 0; i < SampleAssets::SamepleCount; i++)
	{
		FMesh* Mesh = new FMesh(SampleAssets::SampleResources[i], SampleAssets::SampleResourceTexture[i],
			SampleAssets::SampleResourceTexture[i] != "" ? SampleAssets::PsoUseTexture : SampleAssets::PsoNoTexture);

		Mesh->InitMaterial(SampleAssets::SampeMats[i].Name, SampleAssets::SampeMats[i].DiffuseAlbedo, SampleAssets::SampeMats[i].FresnelR0,
			SampleAssets::SampeMats[i].Roughness);
		Scene->AddMeshToScene(Mesh);
	}

	//all characters use one skeleton asset
	LSkeleton* Skeleton = new LSkeleton();
	LAssetDataLoader::LoadSkeletonFromFile(SampleAssets::SkeletonResource, Skeleton);
	ResourceSkeletons.push_back(Skeleton);

	for (UINT i = 0; i < SampleAssets::SampleSkeletalMeshCount; i++)
	{
		//character has skeleton mesh and animator instance
		LCharacter* Character = new LCharacter();

		//create skeletal mesh, skeleton save in skeletal mesh
		FSkeletalMesh* SkeletalMesh = new FSkeletalMesh();
		LAssetDataLoader::LoadSkeletalMeshVertexDataFromFile(SampleAssets::SkeletalMeshResource[i], SkeletalMesh);
		SkeletalMesh->SetSkeleton(Skeleton);

		//TODO: will replace by LTexture later
		FTexture* Tex = GRHI->RHICreateTexture();
		Tex->InitializeTexture(SampleAssets::SkeletonTexture);
		SkeletalMesh->SetDiffuseTexture(Tex);

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
		Scene->AddCharacterToScene(Character);
	}

	// load scene light
	vector<DirectionLightData> DirectionLights;
	LAssetDataLoader::LoadDirectionLights(SampleAssets::SceneLightsFile, DirectionLights);
	for (size_t i = 0; i < DirectionLights.size(); i++)
	{
		FLight* Light = new FLight();
		Light->Direction = DirectionLights[i].Direction;
		Light->Position = DirectionLights[i].Position;
		Light->Strength = DirectionLights[i].Strength;
		Light->LightIndex = (UINT)i;
		Scene->AddLightToScene(Light);
	}

	//load scene camera
	LCamera* SceneCamera = new LSceneCamera();
	LAssetDataLoader::LoadCameraDataFromFile(SampleAssets::CameraBin, SceneCamera);

	LCamera* ThirdPersonCamera = new LThirdPersonCamera();
	LAssetDataLoader::LoadCameraDataFromFile(SampleAssets::CameraBin, ThirdPersonCamera);
	ThirdPersonCamera->SetSocketOffset(XMFLOAT3(-2.f, 0.3f, 2.2f));

	Scene->AddCamera(SceneCamera);
	Scene->AddCamera(ThirdPersonCamera);

	//set the first character is the player
	LPlayerController* PlayerController = new LPlayerController();
	PlayerController->Possess(Scene->GetCharacters()[0]);
	ThirdPersonCamera->SetViewTarget(Scene->GetCharacters()[0]);

	Scene->ActiveCamera(0);
}
