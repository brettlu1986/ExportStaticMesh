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

	void InitRenderResource();
	void UpdateCameraMatrix(float x, float y);
	void Update();
	void Render();
	void EndRender();
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
	
private: 
	std::vector<FMesh*> Meshes;
	UINT MeshCount;
	UINT MeshWithTextureCount;

	LCamera Camera;
};
