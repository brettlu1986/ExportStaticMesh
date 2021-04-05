#pragma once

#include <vector>

class FMesh;
class FRHICommandList;
class FScene
{
public: 
	FScene();
	~FScene();

	void AddMeshToScene(FMesh* Mesh);

	void InitRenderResource();
	void Render(FRHICommandList& CommandList);
	void EndRender();
	void Destroy();
private: 
	std::vector<FMesh*> Meshes;
};
