#pragma once

#include "stdafx.h"
#include "FRenderer.h"
#include "FResourceViewCreater.h"

class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	void Initialize(FScene* RenderScene);
	void RenderScene(FScene* RenderScene);

private:
	FResourceView* RenderTargets[RENDER_TARGET_COUNT];
};
