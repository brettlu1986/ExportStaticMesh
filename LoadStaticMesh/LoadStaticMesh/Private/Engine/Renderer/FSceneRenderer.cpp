
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

	GRHI->DrawSceneToShadowMap(RenderScene);

	GRHI->RenderSceneObjects(RenderScene);
	
	GRHI->EndRenderScene();
}

