
#include "stdafx.h"
#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "LCamera.h"
#include "LSceneCamera.h"
//#include "FShadowMap.h"
#include "FLight.h"

#include "LThirdPersonCamera.h"
#include "LPlayerController.h"

FScene::FScene()
//:ConstantDirty(1)
//,ShadowMap(nullptr)
{
	//Init();
}

FScene::~FScene()
{
	Destroy();
}

//void FScene::Init()
//{
//	//ShadowMap = new FShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
//}

//void FScene::ActiveCamera(UINT CameraIndex)
//{
	//if(CameraIndex > Cameras.size() -1)
	//{
	//	assert(!"camera index should not above camera size");
	//}

	//for(UINT i = 0 ; i < Cameras.size(); i++)
	//{
	//	Cameras[i]->SetActive(CameraIndex == i);
	//}
//}

void FScene::Destroy()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Destroy();
	}
	Meshes.clear();

	delete PassViewProj;
	PassViewProj = nullptr;

	delete PassLightInfo;
	PassLightInfo = nullptr;

	//for (size_t i = 0; i < Players.size(); ++i)
	//{
	//	Players[i]->Destroy();
	//}
	//Players.clear();

	//vector<LCamera*>::iterator it = Cameras.begin();
	//for(; it < Cameras.end(); it++)
	//{
	//	LCamera* Ca = *it;
	//	delete Ca;
	//}

	//if(ShadowMap)
	//{
	//	delete ShadowMap;
	//	ShadowMap = nullptr;
	//}
}

void FScene::UpdateViewProjInfo(FPassViewProjection UpdateViewProj)
{
	ViewProjInfo = UpdateViewProj;
	GRHI->UpdateConstantBufferView(PassViewProj, &ViewProjInfo);
}

void FScene::UpdateLightInfo(FPassLightInfo UpdateLight)
{
	LightInfo = UpdateLight;
	GRHI->UpdateConstantBufferView(PassLightInfo, &LightInfo);
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	UINT Len = (UINT)Meshes.size();
	Mesh->SetMeshIndex(Len);
	Meshes.push_back(Mesh);
}

void FScene::AddLightToScene(FLight* Light)
{
	SceneLights.push_back(Light);
}

void FScene::UpdateLightToScene(FLight* Light)
{
	UINT LightIndex = Light->GetLightIndex();
	if(LightIndex == 0 )
	{
		FPassLightInfo LightInfo;
		XMFLOAT3 Pos = Light->GetPosition();
		XMFLOAT3 Dir = Light->GetDir();
		LightInfo.Lights[LightIndex].Direction = Dir;
		LightInfo.Lights[LightIndex].Strength = Light->GetStrength();
		LightInfo.Lights[LightIndex].Position = Pos;

		XMFLOAT3 LightUp = { 0, 0, 1 };
		
		XMMATRIX LightView = XMMatrixLookToLH(XMLoadFloat3(&Pos), XMLoadFloat3(&Dir), XMLoadFloat3(&LightUp));
		XMMATRIX LightProj = XMMatrixOrthographicLH((float)50, (float)50, 1.f, 100.f);
		XMMATRIX LightSpaceMatrix = LightView * LightProj;

		XMStoreFloat4x4(&LightInfo.LightSpaceMatrix, XMMatrixTranspose(LightSpaceMatrix));

		// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		XMMATRIX T(
			0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f);
		XMMATRIX S = LightView * LightProj * T;
		XMStoreFloat4x4(&LightInfo.ShadowTransform, XMMatrixTranspose(S));

		UpdateLightInfo(LightInfo);
	}
}

void FScene::AddSkeletalMeshToScene(FSkeletalMesh* Mesh)
{
	SkmMeshes.push_back(Mesh);
}

//void FScene::AddCharacterToScene(LCharacter* Character)
//{
	//Players.push_back(Character);
	//SkmMeshes.push_back(Character->GetSkeletalMesh());
//}

//void FScene::AddLightToScene(FLight* Light)
//{
	//SceneLights.push_back(Light);
	//if(Light->LightIndex == 0)
	//{
	//	Light->Init();
	//}
//}

//void FScene::Update(float DeltaTime)
//{
	//GetActiveCamera()->Update(DeltaTime);

	//for(LCharacter* Ch : Players)
	//{
	//	Ch->Update(DeltaTime);
	//}

	//FLight* Light = GetLight(0);
	//Light->Update(DeltaTime);
//}


