
#include "stdafx.h"
#include "FSceneRenderer.h"

FSceneRenderer::FSceneRenderer()
{
}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderFrameResource(FFrameResource& FrameResource)
{
	GRHI->RHIRenderFrameResource(FrameResource);
}