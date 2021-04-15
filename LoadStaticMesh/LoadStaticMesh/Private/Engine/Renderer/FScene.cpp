#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"
#include "LCamera.h"

FScene::FScene()
:MeshCount(0)
,MeshWithTextureCount(0)
{

}

FScene::~FScene()
{
	Destroy();
}

void FScene::Destroy()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Destroy();
	}
	Meshes.clear();
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	Mesh->SetMatrixCbIndex(MeshCount);
	Mesh->SetMaterialCbvHeapIndex(MeshCount);
	if(Mesh->GetDiffuseTexture())
	{
		Mesh->SetDiffuseTextureHeapIndex(MeshWithTextureCount);
		MeshWithTextureCount ++;
	}
	Meshes.push_back(Mesh);
	MeshCount++;
}

void FScene::InitRenderResource()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->InitRenderResource();
	}
}

void FScene::Update()
{

}

void FScene::UpdateCameraMatrix(float x, float y)
{	
	Camera.ChangeViewMatrixByMouseEvent(x, y);
}

void FScene::Render()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Render();
	}
}

void FScene::EndRender()
{
	for (size_t i = 0; Meshes.size(); ++i)
	{
		Meshes[i]->EndRender();
	}
}
