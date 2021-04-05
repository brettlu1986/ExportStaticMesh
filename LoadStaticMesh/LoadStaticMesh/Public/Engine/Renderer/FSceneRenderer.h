#pragma once

#include "stdafx.h"
#include "FRenderer.h"


class FScene;
class RHICommandList;
class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	FSceneRenderer(UINT InWidth, UINT InHeight);
	virtual ~FSceneRenderer();

	virtual void RenderInit(FScene* Scene) override;
	virtual void EndRenderFram(FScene* Scene) override;
	virtual void Render(FScene* Scene) override;

private:
	UINT Width;
	UINT Height;
};
