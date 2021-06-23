#pragma once

#include "pch.h"
#include "FRenderer.h"
#include "FResourceViewCreater.h"
#include "FShadowMap.h"
#include "FPostResources.h"

class FSceneRenderer : public FRenderer
{
public: 
	FSceneRenderer();
	virtual ~FSceneRenderer();

	void Destroy();
	void Initialize(FScene* RenderScene);
	void RenderScene(FScene* RenderScene);

private:

	void RenderSceneStaticMeshes(FScene* RenderScene, bool bTranparency = false, bool bShadowPass = false);
	void RenderSceneSkeletalMeshes(FScene* RenderScene, bool bShadowPass = false);

	//default
	FResourceView* RenderTargets[RENDER_TARGET_COUNT];
	FTexture* DsvTex;
	FResourceView* DsvView;
	
	//shadow map
	FShadowMap* ShadowMap;

	//post progress
	FPostResources* PostResoures;
};
