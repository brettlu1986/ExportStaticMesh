#pragma once

#include "stdafx.h"
#include "FRenderer.h"


class FScene;
class RHICommandList;
class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	virtual void RenderInit(FScene* Scene) override;
	virtual void EndRenderFram(FScene* Scene) override;
	virtual void Render(FScene* Scene) override;

};
