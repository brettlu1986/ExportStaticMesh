#pragma once

#include "stdafx.h"

class FMesh;
class FRHICommandList;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);

	void InitRenderResource();
	void Render();
	void EndRender();
	void Destroy();

	UINT GetDrawObjectsNum()
	{
		return static_cast<UINT>(Meshes.size());
	}
	const std::vector<FMesh*>& GetDrawMeshes()
	{
		return Meshes;
	}
	
private: 
	std::vector<FMesh*> Meshes;
};
