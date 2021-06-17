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
	void RenderSceneStaticMeshes(FScene* RenderScene, bool bTranparency = false, bool bShadowPass = false);
	void RenderSceneSkeletalMeshes(FScene* RenderScene, bool bShadowPass = false);

	FResourceView* RenderTargets[RENDER_TARGET_COUNT];

	FShadowMap* ShadowMap;
	FTexture* DsvTex;
	FResourceView* DsvView;

	//
	FTexture* SceneColorTex;
	FResourceView* SRVSceneColor;
	FResourceView* RTVSceneColor;

	LIndexBuffer* FullScreenIBData;
	LVertexBuffer* FullScreenVBData;
	FIndexBuffer* FullScreenIB;
	FVertexBuffer* FullScreenVB;

	//bloom setup
	FTexture* BloomSetupTex;
	FResourceView* RTVBloomSetup;
	FResourceView* CBVBloomSetup;
	FResourceView* SRVBloomSetup;
	FRHIViewPort SetUpViewPort;
	
	//bloom down
	static const UINT DOWN_SAMPLE_COUNT = 4;

	FRHIViewPort BloomDowmViewPort[DOWN_SAMPLE_COUNT];
	FTexture* BloomDownTex[DOWN_SAMPLE_COUNT];
	FResourceView* SRVBloomDown[DOWN_SAMPLE_COUNT];
	FResourceView* RTVBloomDown[DOWN_SAMPLE_COUNT];
	FResourceView* CBVBloomDown[DOWN_SAMPLE_COUNT];

	//bloom up
	static const UINT UP_SAMPLE_COUNT = 3;
	FRHIViewPort BloomUpViewPort[UP_SAMPLE_COUNT];
	FTexture* BloomUpTex[UP_SAMPLE_COUNT];
	FResourceView* SRVBloomUp[UP_SAMPLE_COUNT];
	FResourceView* RTVBloomUp[UP_SAMPLE_COUNT];
	FResourceView* CBVBloomUp[UP_SAMPLE_COUNT];

	//sunmerge
	FTexture* SunMergeTex;
	FResourceView* RTVSunMerge;
	FResourceView* CBVSunMerge;

	//Tonemap 
	FTexture* ToneMapTex;
	FResourceView* RTVToneMap;

};
