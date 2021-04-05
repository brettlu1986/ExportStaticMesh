#include "FScene.h"
#include "FMesh.h"
#include "FRHI.h"

FScene::FScene()
{

}

FScene::~FScene()
{

}

void FScene::Destroy()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->Destroy();
	}
}

void FScene::AddMeshToScene(FMesh* Mesh)
{
	Meshes.push_back(Mesh);
}

void FScene::InitRenderResource()
{
	for (size_t i = 0; i < Meshes.size(); ++i)
	{
		Meshes[i]->InitRenderResource();
	}
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
