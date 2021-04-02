#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::BeginRenderFrame(FScene* Scene)
{
	Scene->InitRenderResource();
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render(FRHICommandList& CommandList, FScene* Scene) 
{
	Scene->Render(CommandList);
}