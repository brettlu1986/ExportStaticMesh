#pragma once

#include "LMarcoDef.h"
#include "FBloomDef.h"
#include "FTexture.h"
#include "FResourceViewCreater.h"
#include "FRHI.h"
#include "FBloomSetup.h"
#include "FBloomDown.h"
#include "FBloomUp.h"
#include "FSunMerge.h"
#include "FToneMapping.h"

class FPostResources
{
public:
	FPostResources()
		:SceneColorTex(nullptr)
		, SRVSceneColor(nullptr)
		, RTVSceneColor(nullptr)
		, BloomSetup(nullptr)
		, BloomDown(nullptr)
		, BloomUp(nullptr)
		, SunMerge(nullptr)
		, ToneMap(nullptr)
	{
		
	};

	~FPostResources()
	{
		//scene color
		SAFE_DESTROY(SceneColorTex);
		SAFE_DELETE(SRVSceneColor);
		SAFE_DELETE(RTVSceneColor);

		SAFE_DELETE(BloomSetup);
		SAFE_DELETE(BloomDown);
		SAFE_DELETE(BloomUp);
		SAFE_DELETE(SunMerge);
		SAFE_DELETE(ToneMap)
	};

	void Initialize()
	{	
		FClearValue SceneClearValue;
		SceneClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT;
		SceneClearValue.ClearDepth = nullptr;
		SceneClearValue.ClearColor = &PostClearColor;

		FTextureInitializer SceneColorInitializer =
		{
			(UINT)GRHI->GetDefaultViewPort().Width, (UINT)GRHI->GetDefaultViewPort().Height, 1, 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &SceneClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		SceneColorTex = GRHI->CreateTexture(SceneColorInitializer);
		SRVSceneColor = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &SceneColorTex,
			0, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT });
		RTVSceneColor = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &SceneColorTex, 0, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT });

		BloomSetup = new FBloomSetup();
		BloomSetup->Initialize(this);

		BloomDown = new FBloomDown();
		BloomDown->Initialize(this);

		BloomUp = new FBloomUp();
		BloomUp->Initialize(this);

		SunMerge = new FSunMerge();
		SunMerge->Initialize(this);

		ToneMap = new FToneMapping();
		ToneMap->Initialize(this);
	}

	FBloomSetup* GetBloomSetup()
	{
		return BloomSetup;
	}

	FBloomUp* GetBloomUp()
	{
		return BloomUp;
	}

	FBloomDown* GetBloomDown()
	{
		return BloomDown;
	}

	FSunMerge* GetSunMerge()
	{
		return SunMerge;
	}

	FToneMapping* GetToneMapping()
	{
		return ToneMap;
	}

	FTexture* SceneColorTex;
	FResourceView* SRVSceneColor;
	FResourceView* RTVSceneColor;

private:
	FBloomSetup* BloomSetup;
	FBloomDown* BloomDown;
	FBloomUp* BloomUp;
	FSunMerge* SunMerge;
	FToneMapping* ToneMap;
};
