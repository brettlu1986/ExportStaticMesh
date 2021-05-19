#pragma once

#include "stdafx.h"
#include "LCamera.h"
#include "FResourceViewCreater.h"
#include "LCharacter.h"
#include "FSkeletalMesh.h"

class FMesh;
class LCamera;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);
	void AddCharacterToScene(LCharacter* Character);
	void AddLightToScene(FLight* Light);
	void InitCharacters();

	void InitSceneRenderResource();

	void Update(float DeltaTime);
	void Destroy();

	UINT GetMeshCount()
	{
		return MeshCount;
	}

	UINT GetMeshWithTextureNum()
	{
		return MeshWithTextureCount;
	}

	const std::vector<FMesh*>& GetDrawMeshes()
	{
		return Meshes;
	}

	const std::vector<FSkeletalMesh*>& GetDrawSkeletalMeshes() 
	{
		return SkmMeshes;
	}

	const std::vector<LCharacter*>& GetCharacters()
	{
		return Players;
	}

	LCamera& GetCamera()
	{
		return Camera;
	}
	
	bool IsConstantDirty()
	{
		return ConstantDirty > 0;
	}

	void DecreaseDirtyCount()
	{
		ConstantDirty --;
	}

	FLight* GetLight(UINT LightIndex)
	{
		return SceneLights[LightIndex];
	}

	FResourceView* PassContantView;

private: 
	std::vector<LCharacter*> Players;
	std::vector<FSkeletalMesh*> SkmMeshes;
	std::vector<FMesh*> Meshes;
	std::vector<FLight*> SceneLights;

	UINT MeshCount;
	UINT MeshWithTextureCount;
	UINT ConstantDirty;
	LCamera Camera;
};
