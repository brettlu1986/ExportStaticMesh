#pragma once

#include "stdafx.h"
#include "FRenderer.h"
#include "FResourceViewCreater.h"
#include "FShadowMap.h"

class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	void Initialize(FScene* RenderScene);
	void RenderScene(FScene* RenderScene);

private:
	FResourceView* RenderTargets[RENDER_TARGET_COUNT];

	FShadowMap* ShadowMap;
	FTexture* DsvTex;
	FResourceView* DsvView;

};
