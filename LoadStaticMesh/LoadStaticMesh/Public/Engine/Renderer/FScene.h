#pragma once

#include "stdafx.h"
#include "LCamera.h"

class FMesh;
class LCamera;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);
	void InitSceneRenderResource();

	void UpdateCameraMatrix(float x, float y);
	void UpdateCameraDistanceByKey(UINT8 Key);
	void Update();
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
private: 
	std::vector<FMesh*> Meshes;
	UINT MeshCount;
	UINT MeshWithTextureCount;
	UINT ConstantDirty;
	LCamera Camera;
};
