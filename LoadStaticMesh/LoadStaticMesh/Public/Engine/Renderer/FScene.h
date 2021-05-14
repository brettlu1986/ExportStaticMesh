#pragma once

#include "stdafx.h"
#include "LCamera.h"

class FMesh;
class LCamera;
class LCharacter;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);
	void AddCharacterToScene(LCharacter* Character);
	void AddLightToScene(FLight* Light);

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

private: 
	std::vector<LCharacter*> Players;
	std::vector<FMesh*> Meshes;
	std::vector<FLight*> SceneLights;
	UINT MeshCount;
	UINT MeshWithTextureCount;
	UINT ConstantDirty;
	LCamera Camera;

	// temp use to rotate light
	float Alpha;
	float Theta;
	float Radius;
};
