
#include "stdafx.h"
#include "FSceneRenderer.h"

FSceneRenderer::FSceneRenderer()
{
}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	const std::vector<FMesh*> Meshes = RenderScene->GetDrawMeshes();
	for(size_t i = 0; i < Meshes.size(); i++)
	{
		GRHI->DrawMesh(Meshes[i]);
	}
}

