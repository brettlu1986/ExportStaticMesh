
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
	GRHI->BeginRenderScene();

	GRHI->DrawSceneToShaderMap(RenderScene);

	GRHI->RenderSceneObjects(RenderScene);
	
	GRHI->EndRenderScene();
}

