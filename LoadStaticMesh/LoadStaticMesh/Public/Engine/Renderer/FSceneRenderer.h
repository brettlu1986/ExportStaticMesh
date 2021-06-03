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

	void Destroy();
	void Initialize(FScene* RenderScene);
	void RenderScene(FScene* RenderScene);

private:
	void RenderSceneStaticMeshes(FScene* RenderScene, bool bShadowPass = false);
	void RenderSceneSkeletalMeshes(FScene* RenderScene);

	FResourceView* RenderTargets[RENDER_TARGET_COUNT];

	FShadowMap* ShadowMap;
	FTexture* DsvTex;
	FResourceView* DsvView;

};
