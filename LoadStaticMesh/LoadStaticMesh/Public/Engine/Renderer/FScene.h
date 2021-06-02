#pragma once

#include "stdafx.h"
#include "LCamera.h"
#include "FResourceViewCreater.h"
#include "LCharacter.h"
#include "FSkeletalMesh.h"

//class FShadowMap;
class FMesh;
class FLight;
//class LCamera;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);
	void AddSkeletalMeshToScene(FSkeletalMesh* Mesh);
	//void AddCharacterToScene(LCharacter* Character);
	void AddLightToScene(FLight* Light);
	void UpdateLightToScene(FLight* Light);

	//void Update(float DeltaTime);
	void Destroy();

	const vector<FMesh*>& GetDrawMeshes()
	{
		return Meshes;
	}


	const vector<FSkeletalMesh*>& GetDrawSkeletalMeshes() 
	{
		return SkmMeshes;
	}

	//const vector<LCharacter*>& GetCharacters()
	//{
	//	return Players;
	//}

	//LCamera* GetActiveCamera()
	//{
	//	for(LCamera* Camera : Cameras)
	//	{
	//		if(Camera->IsActive())
	//			return Camera;
	//	}
	//	assert(!"must have one active camera");
	//	return Cameras[0];
	//}

	//void AddCamera(LCamera* Ca)
	//{
	//	Cameras.push_back(Ca);
	//}
	
	//bool IsConstantDirty()
	//{
	//	return ConstantDirty > 0;
	//}

	//void DecreaseDirtyCount()
	//{
	//	ConstantDirty--;
	//}

	//FLight* GetLight(UINT LightIndex)
	//{
	//	return SceneLights[LightIndex];
	//}

	//LCharacter* GetLocalControlPlayer()
	//{
	//	return Players[0];
	//}

	FResourceView* PassViewProj;
	FResourceView* PassLightInfo;

	//void ActiveCamera(UINT CameraIndex);

	//LCamera* GetThirdPersonCamera()
	//{
	//	for (LCamera* Camera : Cameras)
	//	{
	//		if (Camera->GetCameraType() == E_CAMERA_TYPE::CAMERA_THIRD_PERSON)
	//			return Camera;
	//	}
	//	return nullptr;
	//}

	void UpdateViewProjInfo(FPassViewProjection UpdateViewProj);
	void UpdateLightInfo(FPassLightInfo UpdateLight);
	

	//FShadowMap* GetShadowMap()
	//{
	//	return ShadowMap;
	//}
private: 
	//void Init();
	
	//vector<LCharacter*> Players;
	vector<FSkeletalMesh*> SkmMeshes;
	vector<FMesh*> Meshes;
	vector<FLight*> SceneLights;

	//UINT ConstantDirty;

	//vector<LCamera*> Cameras;
	//FShadowMap* ShadowMap;

	FPassViewProjection ViewProjInfo;
	FPassLightInfo LightInfo;
};
