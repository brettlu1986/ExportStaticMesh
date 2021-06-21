
#include "stdafx.h"
#include "FSceneRenderer.h"
#include "FSkeletalMesh.h"
#include "LCharacter.h"
#include "FShadowMap.h"
#include "LDeviceWindows.h"
#include "LEngine.h"
#include "LLog.h"

FSceneRenderer::FSceneRenderer()
:DsvTex(nullptr)
,DsvView(nullptr)
, SceneColorTex(nullptr)
, SRVSceneColor(nullptr)
, RTVSceneColor(nullptr)
, FullScreenIBData(nullptr)
, FullScreenVBData(nullptr)
, FullScreenIB(nullptr)
, FullScreenVB(nullptr)
, SRVBloomSetup(nullptr)
, BloomSetupTex(nullptr)
, RTVBloomSetup(nullptr)
, CBVBloomSetup(nullptr)
, SunMergeTex(nullptr)
, RTVSunMerge(nullptr)
, CBVSunMerge(nullptr)
, SRVSunMerge(nullptr)
, ShadowMap(nullptr)
, ToneMapTex(nullptr)
, RTVToneMap(nullptr)
, SRVToneMap(nullptr)
{
	for (UINT i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		RenderTargets[i] = nullptr;
	}

	//bloom down
	for (UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		BloomDownTex[i] = nullptr;
		RTVBloomDown[i] = nullptr;
		CBVBloomDown[i] = nullptr;
		SRVBloomDown[i] = nullptr;
	}

	//bloom up
	for (UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		BloomUpTex[i] = nullptr;
		SRVBloomUp[i] = nullptr;
		RTVBloomUp[i] = nullptr;
		CBVBloomUp[i] = nullptr;
	}

}

FSceneRenderer::~FSceneRenderer()
{
	
}

void FSceneRenderer::Destroy()
{
	for (UINT i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		delete RenderTargets[i];
		RenderTargets[i] = nullptr;
	}

	SAFE_DESTROY(DsvTex);
	SAFE_DELETE(DsvView)

	//scene color
	SAFE_DESTROY(SceneColorTex);
	SAFE_DELETE(SRVSceneColor);
	SAFE_DELETE(RTVSceneColor);

	//bloom setup
	SAFE_DELETE(FullScreenIBData);
	SAFE_DELETE(FullScreenVBData);

	SAFE_DELETE(FullScreenIB);
	SAFE_DELETE(FullScreenVB);

	SAFE_DESTROY(BloomSetupTex);
	SAFE_DELETE(SRVBloomSetup);
	SAFE_DELETE(RTVBloomSetup);
	SAFE_DELETE(CBVBloomSetup);

	//bloom down 
	for(UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
	{
		SAFE_DESTROY(BloomDownTex[i]);
		SAFE_DELETE(RTVBloomDown[i]);
		SAFE_DELETE(CBVBloomDown[i]);
		SAFE_DELETE(SRVBloomDown[i]);
	}

	//bloom up
	for(UINT i = 0; i < UP_SAMPLE_COUNT; i++)
	{
		SAFE_DESTROY(BloomUpTex[i]);
		SAFE_DELETE(RTVBloomUp[i]);
		SAFE_DELETE(CBVBloomUp[i]);
		SAFE_DELETE(SRVBloomUp[i]);
	}

	//sunmerge 
	SAFE_DESTROY(SunMergeTex);
	SAFE_DELETE(SRVSunMerge);
	SAFE_DELETE(RTVSunMerge);
	SAFE_DELETE(CBVSunMerge);

	//tone map
	SAFE_DESTROY(ToneMapTex);
	SAFE_DELETE(RTVToneMap);
	SAFE_DELETE(SRVToneMap);

	//shadow map
	SAFE_DESTROY(ShadowMap);
}

void FSceneRenderer::Initialize(FScene* RenderScene)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	{
		//create use to create rtv, uav, srv
		const UINT CbvCount = 50;
		const UINT SrvCount = 100;
		const UINT UavCount = 10;
		const UINT DsvCount = 10;
		const UINT RtvCount = 60;
		const UINT SamplerCount = 20;
		GRHI->CreateResourceViewCreater(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);
	}
	
	{
		//create swap render targets
		for (UINT i = 0; i < RENDER_TARGET_COUNT; ++i)
		{
			RenderTargets[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, nullptr, 0, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM, i });
		}

		//create default dsv view
		FClearDepth ClearDepth;
		ClearDepth.Depth = 1.0f;
		ClearDepth.Stencil = 0;

		FClearValue ClearValue;
		ClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT;
		ClearValue.ClearDepth = &ClearDepth;
		ClearValue.ClearColor = nullptr;
		FTextureInitializer Initializer =
		{
			(UINT)GRHI->GetDefaultViewPort().Width, (UINT)GRHI->GetDefaultViewPort().Height, 1, 0, E_GRAPHIC_FORMAT::FORMAT_R24G8_TYPELESS, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &ClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE
		};
		DsvTex = GRHI->CreateTexture(Initializer);
		DsvView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV, 1, &DsvTex, 0, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });
	}
	
	{
		//create used pipline state objects
		GRHI->CreatePipelineStateObject({ "PsoUseTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), "ShaderTexVs",
			"ShaderTexPs", 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, FRHIRasterizerState(), true, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });
		GRHI->CreatePipelineStateObject({ "PsoNoTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), "ShaderVs",
			"ShaderPs", 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, FRHIRasterizerState(),false,E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });

		GRHI->CreatePipelineStateObject({ "HighLight", StandardInputElementDescs, _countof(StandardInputElementDescs), "ShaderVs",
		"ShaderHighLightPs", 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, FRHIRasterizerState(),false, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });

		FRHIRasterizerState State;
		State.DepthBias = 25000;
		State.DepthBiasClamp = 0.f;
		State.SlopeScaledDepthBias = 0.1f;
		GRHI->CreatePipelineStateObject({ "ShadowPass", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), "SampleDepthShaderVs",
			"SampleDepthShaderPs", 0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN, State, false, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });

		GRHI->CreatePipelineStateObject({ "SkinShadowPass", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), "SampleDepthSkinVs",
			"SampleDepthSkinPs", 0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN, State, false, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });

		GRHI->CreatePipelineStateObject({ "SKMPso", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), "SkeletalVs",
			"SkeletalPs", 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, FRHIRasterizerState(), false, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });
	}
	
	{
		//scene color render target, HDR
		FClearColor SceneClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };

		FClearValue SceneClearValue;
		SceneClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT;
		SceneClearValue.ClearDepth = nullptr;
		SceneClearValue.ClearColor = &SceneClearColor;

		FTextureInitializer SceneColorInitializer =
		{
			(UINT)GRHI->GetDefaultViewPort().Width, (UINT)GRHI->GetDefaultViewPort().Height, 1, 1, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &SceneClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		SceneColorTex = GRHI->CreateTexture(SceneColorInitializer);
		SRVSceneColor = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &SceneColorTex,
			0, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT });
		RTVSceneColor = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &SceneColorTex, 0, E_GRAPHIC_FORMAT::FORMAT_R16G16B16A16_FLOAT });

	}
	
	const FRHIInputElement FullInputElementDescs[] =
	{
		{ "POSITION", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32B32_FLOAT, 0, 0, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, E_GRAPHIC_FORMAT::FORMAT_R32G32_FLOAT, 0, 12, ERHI_INPUT_CLASSIFICATION::INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	static const auto BloomRTFormat = E_GRAPHIC_FORMAT::FORMAT_R11G11B10_FLOAT;

	UINT SetUpWidth = (UINT)(GRHI->GetDefaultViewPort().Width / 4);
	UINT SetUpHeight = (UINT)(GRHI->GetDefaultViewPort().Height / 4);

	static const float BloomIntensity = 1.0f;
	static const float BloomThreHold = 1.0f;

	static const XMFLOAT4 BloomTint1 = { 0.3465f, 0.3465f, 0.3465f, 0.3465f };
	static const XMFLOAT4 BloomTint2 = { 0.138f, 0.138f, 0.138f, 0.138f };
	static const XMFLOAT4 BloomTint3 = { 0.1176f,  0.1176f, 0.1176f, 0.1176f };
	static const XMFLOAT4 BloomTint4 = { 0.066f,  0.066f, 0.066f, 0.066f };
	static const XMFLOAT4 BloomTint5 = { 0.066f,  0.066f, 0.066f, 0.066f };

	static const XMFLOAT4 BloomTintAs[] = { BloomTint4,
		XMFLOAT4(BloomTint3.x * BloomIntensity, BloomTint3.y * BloomIntensity, BloomTint3.z * BloomIntensity, BloomTint3.w * BloomIntensity),
		XMFLOAT4(BloomTint2.x * BloomIntensity, BloomTint2.y * BloomIntensity, BloomTint2.z * BloomIntensity, BloomTint2.w * BloomIntensity)
	};
	static const XMFLOAT4 BloomTintBs[] = { BloomTint5,
		XMFLOAT4(1.f, 1.f, 1.f, 0.f),
		XMFLOAT4(1.f, 1.f, 1.f, 0.f),
	};

	//create setup render target
	FClearColor BloomClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
	FClearValue BloomClearValue;
	BloomClearValue.Format = BloomRTFormat;
	BloomClearValue.ClearDepth = nullptr;
	BloomClearValue.ClearColor = &BloomClearColor;

	{
		//bloom setup, full screen mesh
		static const float Vertices[] = {
				1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 3.0f, 0.0f, 1.0f, -1.0f,
				-3.0f, -1.0f, 0.0f, -1.0f, 1.0f };
		static UINT Indices[] = { 0, 1, 2 };

		FullScreenVBData = new LVertexBuffer((char*)Vertices, sizeof(Vertices), 3);
		FullScreenIBData = new LIndexBuffer(3, sizeof(UINT) * 3, E_INDEX_TYPE::TYPE_UINT_32, reinterpret_cast<void*>(Indices));

		//FullScreenVB = GRHI->RHICreateVertexBuffer();
		//FullScreenIB = GRHI->RHICreateIndexBuffer();

		//GRHI->UpdateVertexBufferResource(FullScreenVB, *FullScreenVBData);
		//GRHI->UpdateIndexBufferResource(FullScreenIB, *FullScreenIBData);

		GRHI->CreatePipelineStateObject({ "BloomSetup", FullInputElementDescs, _countof(FullInputElementDescs), "BloomSetupVs",
			"BloomSetupPs", 1, BloomRTFormat, FRHIRasterizerState() });

		//create setup render target
		SetUpViewPort = FRHIViewPort(0.f, 0.f, (float)SetUpWidth, (float)SetUpHeight);
		FTextureInitializer SetupTexInitializer =
		{
			SetUpWidth, SetUpHeight, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		BloomSetupTex = GRHI->CreateTexture(SetupTexInitializer);
		SRVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomSetupTex,
			0, BloomRTFormat });
		RTVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomSetupTex, 0, BloomRTFormat });
		CBVBloomSetup = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomSetup)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
		
		FPassBloomSetup PassBloomSetup;
		PassBloomSetup.BufferSizeAndInvSize = XMFLOAT4(static_cast<float>(SetUpWidth), static_cast<float>(SetUpHeight), 1.f/ SetUpWidth, 1.f/ SetUpHeight);
		PassBloomSetup.BloomThrehold = BloomThreHold;
		GRHI->UpdateConstantBufferView(CBVBloomSetup, &PassBloomSetup);
	}

	{
		//bloom down
		GRHI->CreatePipelineStateObject({ "BloomDown", FullInputElementDescs, _countof(FullInputElementDescs), "BloomDownVs",
			"BloomDownPs", 1, BloomRTFormat, FRHIRasterizerState() });

		//create setup render target
		for(UINT i = 0; i< DOWN_SAMPLE_COUNT; i++)
		{
			UINT W = SetUpWidth >> (i + 1);
			UINT H = SetUpHeight >> (i + 1);
			BloomDowmViewPort[i] = FRHIViewPort(0.f, 0.f, (float)W, (float)H);
			FTextureInitializer DownTexInitializer =
			{
				W, H, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
				E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
			};
			BloomDownTex[i] = GRHI->CreateTexture(DownTexInitializer);
			SRVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomDownTex[i],
			0, BloomRTFormat });
			RTVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomDownTex[i], 0, BloomRTFormat });
			CBVBloomDown[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomDown)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
			
			FPassBloomDown PassBloomDown;
			PassBloomDown.BufferSizeAndInvSize = XMFLOAT4(static_cast<float>(W), static_cast<float>(H), 1.f / W, 1.f / H);
			PassBloomDown.BloomDownScale = 0.66f * 4.f;
			GRHI->UpdateConstantBufferView(CBVBloomDown[i], &PassBloomDown);
		}
	}

	{
		//bloom up
		GRHI->CreatePipelineStateObject({ "BloomUp", FullInputElementDescs, _countof(FullInputElementDescs), "BloomUpVs",
			"BloomUpPs", 1, BloomRTFormat, FRHIRasterizerState() });
	
		for(UINT i = 0; i < UP_SAMPLE_COUNT; i++)
		{
			UINT UpW = SetUpWidth >> ( DOWN_SAMPLE_COUNT - 1 - i);
			UINT UpH = SetUpHeight >> (DOWN_SAMPLE_COUNT - 1 - i);
			BloomUpViewPort[i] = FRHIViewPort(0.f, 0.f, (float)UpW, (float)UpH);

			UINT UpCompW = SetUpWidth >> (DOWN_SAMPLE_COUNT - i);
			UINT UpCompH = SetUpHeight >> (DOWN_SAMPLE_COUNT - i);

			FTextureInitializer UpTexInitializer =
			{
				UpW, UpH, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
				E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
			};
			BloomUpTex[i] = GRHI->CreateTexture(UpTexInitializer);
			SRVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &BloomUpTex[i],
			0, BloomRTFormat });
			RTVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &BloomUpTex[i], 0, BloomRTFormat });
			CBVBloomUp[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassBloomUp)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
			
			FPassBloomUp BloomUp;
			BloomUp.BufferASizeAndInvSize = XMFLOAT4(static_cast<float>(UpW), static_cast<float>(UpH), 1.f / UpW, 1.f / UpH);
			BloomUp.BufferBSizeAndInvSize = XMFLOAT4(static_cast<float>(UpCompW), static_cast<float>(UpCompH), 1.f / UpCompW, 1.f / UpCompH);
			float Factor = 1.f / 8.f;
			BloomUp.BloomTintA = XMFLOAT4(BloomTintAs[i].x * Factor, BloomTintAs[i].y * Factor, BloomTintAs[i].z * Factor, BloomTintAs[i].w * Factor);   ;
			BloomUp.BloomTintB = XMFLOAT4(BloomTintBs[i].x * Factor, BloomTintBs[i].y * Factor, BloomTintBs[i].z * Factor, BloomTintBs[i].w * Factor);
			BloomUp.BloomUpScales.x = 0.66f * 2.0f;
			BloomUp.BloomUpScales.y = 0.66f * 2.0f;
			GRHI->UpdateConstantBufferView(CBVBloomUp[i], &BloomUp);
		}

	}

	{
		//sun merge
		GRHI->CreatePipelineStateObject({ "SunMerge", FullInputElementDescs, _countof(FullInputElementDescs), "SunMergeVs",
			"SunMergePs", 1, BloomRTFormat, FRHIRasterizerState() });

		UINT W = SetUpWidth;
		UINT H = SetUpHeight;
		SunMergeViewPort = FRHIViewPort(0.f, 0.f, (float)W, (float)H);
		FTextureInitializer SunMergeTexInitializer =
		{
			W, H, 1, 1, BloomRTFormat, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &BloomClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};
		SunMergeTex = GRHI->CreateTexture(SunMergeTexInitializer);
		SRVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &SunMergeTex,
			0, BloomRTFormat });
		RTVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &SunMergeTex, 0, BloomRTFormat });
		CBVSunMerge = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassSunMerge)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

		FPassSunMerge PassSunMerge;
		//last bloom up size
		UINT UpW = (UINT)BloomUpViewPort[UP_SAMPLE_COUNT - 1].Width;
		UINT UpH = (UINT)BloomUpViewPort[UP_SAMPLE_COUNT - 1].Height;
		PassSunMerge.BloomUpSizeAndInvSize = XMFLOAT4(static_cast<float>(UpW), static_cast<float>(UpH), 1.f / UpW, 1.f / UpH);
		float Factor = BloomIntensity * 0.5f;
		PassSunMerge.BloomColor = XMFLOAT3(BloomTint1.x * Factor, BloomTint1.y * Factor, BloomTint1.z * Factor);
		GRHI->UpdateConstantBufferView(CBVSunMerge, &PassSunMerge);
	}
	
	{
		FClearColor ToneMapClearColor = { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f };
		FClearValue ToneMapClearValue;
		ToneMapClearValue.Format = E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM;
		ToneMapClearValue.ClearDepth = nullptr;
		ToneMapClearValue.ClearColor = &ToneMapClearColor;

		//Tone map
		GRHI->CreatePipelineStateObject({ "ToneMap", FullInputElementDescs, _countof(FullInputElementDescs), "ToneMapVs",
			"ToneMapPs", 1, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });

		FTextureInitializer ToneMapTexInitializer =
		{
			(UINT)GRHI->GetDefaultViewPort().Width, (UINT)GRHI->GetDefaultViewPort().Height, 1, 1, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_RENDER_TARGET, &ToneMapClearValue,
			E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET
		};

		ToneMapTex = GRHI->CreateTexture(ToneMapTexInitializer);
		SRVToneMap = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &ToneMapTex,
			0, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM });
		RTVToneMap = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, &ToneMapTex, 0, E_GRAPHIC_FORMAT::FORMAT_R8G8B8A8_UNORM });
	}

	//shadow map
	{
		ShadowMap = new FShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
		ShadowMap->InitRenderResource();
	}

	RenderScene->PassViewProj = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassViewProjection)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	RenderScene->PassLightInfo = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassLightInfo)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FSceneRenderer::InitFullScreenResource()
{
	FullScreenVB = GRHI->RHICreateVertexBuffer();
	FullScreenIB = GRHI->RHICreateIndexBuffer();

	GRHI->UpdateVertexBufferResource(FullScreenVB, *FullScreenVBData);
	GRHI->UpdateIndexBufferResource(FullScreenIB, *FullScreenIBData);
}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	{
		FUserMarker UserMarker("Render Begin");
		GRHI->SetViewPortInfo(GRHI->GetDefaultViewPort());

		vector<FResourceHeap*> Heaps;
		FResourceViewCreater* ResViewCreater = GRHI->GetResourceViewCreater();
		Heaps.push_back(ResViewCreater->GetCbvSrvUavHeap());
		GRHI->SetResourceHeaps(Heaps);
		GRHI->ResourceTransition(RenderTargets[GRHI->GetFrameIndex()],  E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
		GRHI->SetRenderTargets(RenderTargets[GRHI->GetFrameIndex()], DsvView);
	}

	{
		FUserMarker UserMarker("Shadow Pass");
		GRHI->SetViewPortInfo(ShadowMap->ViewPort);
		GRHI->SetPiplineStateObject(GRHI->GetPsoObject("ShadowPass"));
		GRHI->ResourceTransition(ShadowMap->ShadowResView, E_RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE);
		GRHI->SetRenderTargets(nullptr, ShadowMap->DsvResView);
		RenderSceneStaticMeshes(RenderScene, false, true);
		RenderSceneStaticMeshes(RenderScene, true, true);

		GRHI->SetPiplineStateObject(GRHI->GetPsoObject("SkinShadowPass"));
		RenderSceneSkeletalMeshes(RenderScene, true);
		GRHI->ResourceTransition(ShadowMap->ShadowResView, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
	
	{
		FUserMarker UserMarker("SceneBasePass");
		GRHI->SetViewPortInfo(GRHI->GetDefaultViewPort());
		//save texture
		GRHI->ResourceTransition(SRVSceneColor, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
		GRHI->SetRenderTargets(RTVSceneColor, DsvView);
		// draw opaque static mesh 
		{
			FUserMarker UserMarker("Draw Opaque Static Mesh");
			RenderSceneStaticMeshes(RenderScene, false);
		}
		// draw skeletal mesh 
		{
			FUserMarker UserMarker("Draw Skeletal Mesh");
			RenderSceneSkeletalMeshes(RenderScene);
		}
		// draw transparency static mesh 
		{
			FUserMarker UserMarker("Draw Transparency Static Mesh");
			RenderSceneStaticMeshes(RenderScene, true);
		}
		GRHI->ResourceTransition(SRVSceneColor, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}

	{
		FUserMarker UserMarker("PostProgress");
		{
			FUserMarker UserMarker("Bloom");
			if(FullScreenVB == nullptr || FullScreenIB == nullptr)
			{
				InitFullScreenResource();
			}
			GRHI->SetVertexAndIndexBuffers(FullScreenVB, FullScreenIB);
			{
				FUserMarker UserMarker("Bloom Setup");
				GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomSetup"));
				GRHI->SetViewPortInfo(SetUpViewPort);
				GRHI->ResourceTransition(SRVBloomSetup, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
				GRHI->SetRenderTargets(RTVBloomSetup, nullptr);
				GRHI->SetResourceParams(0, CBVBloomSetup);
				GRHI->SetResourceParams(4, SRVSceneColor);
				GRHI->DrawTriangleList(FullScreenIB);
			}

			{
				FUserMarker UserMarker("Bloom Down");
				GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomDown"));
				for(UINT i = 0; i < DOWN_SAMPLE_COUNT; i++)
				{
					GRHI->SetViewPortInfo(BloomDowmViewPort[i]);
					FResourceView* ResView = nullptr;
					GRHI->ResourceTransition(SRVBloomDown[i], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
					GRHI->SetRenderTargets(RTVBloomDown[i], nullptr);
					GRHI->SetResourceParams(0, CBVBloomDown[i]);
					if (i == 0)
						ResView = SRVBloomSetup;
					else
						ResView = SRVBloomDown[i - 1];
					GRHI->ResourceTransition(ResView, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					GRHI->SetResourceParams(4, ResView);
					GRHI->DrawTriangleList(FullScreenIB);
				}
			}

			{
				FUserMarker UserMarker("Bloom Up");
				GRHI->SetPiplineStateObject(GRHI->GetPsoObject("BloomUp"));
				for(UINT i = 0; i < UP_SAMPLE_COUNT; i++)
				{
					GRHI->SetViewPortInfo(BloomUpViewPort[i]);
					GRHI->ResourceTransition(SRVBloomUp[i], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
					GRHI->SetRenderTargets(RTVBloomUp[i], nullptr);
					GRHI->SetResourceParams(0, CBVBloomUp[i]);
					if(i == 0)
					{
						GRHI->ResourceTransition(SRVBloomDown[UP_SAMPLE_COUNT - 1], E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
						GRHI->ResourceTransition(SRVBloomDown[UP_SAMPLE_COUNT], E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
						GRHI->SetResourceParams(4, SRVBloomDown[UP_SAMPLE_COUNT - 1]);
						GRHI->SetResourceParams(5, SRVBloomDown[UP_SAMPLE_COUNT]);
					}
					else 
					{
						GRHI->ResourceTransition(SRVBloomDown[UP_SAMPLE_COUNT - i - 1],  E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
						GRHI->ResourceTransition(SRVBloomUp[i - 1], E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
						GRHI->SetResourceParams(4, SRVBloomDown[UP_SAMPLE_COUNT - i - 1]);
						GRHI->SetResourceParams(5, SRVBloomUp[i - 1]);
					}
					GRHI->DrawTriangleList(FullScreenIB);
				}
			}

			{
				FUserMarker UserMarker("Sun Merge");
				GRHI->SetViewPortInfo(SunMergeViewPort);
				GRHI->SetPiplineStateObject(GRHI->GetPsoObject("SunMerge"));
				GRHI->ResourceTransition(SRVSunMerge, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
				GRHI->SetRenderTargets(RTVSunMerge, nullptr);
				GRHI->SetResourceParams(0, CBVSunMerge);

				GRHI->ResourceTransition(SRVBloomSetup, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				GRHI->ResourceTransition(SRVBloomUp[UP_SAMPLE_COUNT - 1], E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				GRHI->SetResourceParams(4, SRVBloomSetup);
				GRHI->SetResourceParams(5, SRVBloomUp[UP_SAMPLE_COUNT - 1]);
				GRHI->DrawTriangleList(FullScreenIB);
			}

			{
				FUserMarker UserMarker("Tone Map");
				GRHI->SetViewPortInfo(GRHI->GetDefaultViewPort());
				GRHI->SetPiplineStateObject(GRHI->GetPsoObject("ToneMap"));

				GRHI->ResourceTransition(RenderTargets[GRHI->GetFrameIndex()], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
				GRHI->SetRenderTargets(RenderTargets[GRHI->GetFrameIndex()], nullptr);
			//if there has other post progress, then use RTVToneMap
			//	GRHI->ResourceTransition(SRVToneMap, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
			//	GRHI->SetRenderTargets(RTVToneMap, nullptr);

				GRHI->ResourceTransition(SRVSceneColor, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				GRHI->ResourceTransition(SRVSunMerge, E_RESOURCE_STATE::RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				GRHI->SetResourceParams(4, SRVSceneColor);
				GRHI->SetResourceParams(5, SRVSunMerge);
				GRHI->DrawTriangleList(FullScreenIB);

				
				
			}
		}

	}
	
	GRHI->ResourceTransition(RenderTargets[GRHI->GetFrameIndex()], E_RESOURCE_STATE::RESOURCE_STATE_PRESENT);
}

void FSceneRenderer::RenderSceneStaticMeshes(FScene* RenderScene, bool bTranparency, bool bShadowPass)
{
	vector<FMesh*> Meshes = bTranparency ? RenderScene->GetDrawTransparencyMeshes() : RenderScene->GetDrawMeshes();

	for (size_t i = 0; i < Meshes.size(); i++)
	{
		if (!bShadowPass)
		{	
			FD3DGraphicPipline* Pso = GRHI->GetPsoObject(Meshes[i]->GetPsoKey());
			GRHI->SetPiplineStateObject(Pso);
		}

		GRHI->SetVertexAndIndexBuffers(Meshes[i]->GetVertexBuffer(), Meshes[i]->GetIndexBuffer());
		GRHI->SetResourceParams(0, Meshes[i]->MatrixConstantBufferView);
		GRHI->SetResourceParams(1, Meshes[i]->GetMaterial()->MaterialConstantBufferView);
		GRHI->SetResourceParams(2, RenderScene->PassViewProj);
		GRHI->SetResourceParams(3, RenderScene->PassLightInfo);
		if (Meshes[i]->GetMaterial()->DiffuseResView)
		{
			GRHI->SetResourceParams(4, Meshes[i]->GetMaterial()->DiffuseResView);
		}
		GRHI->SetResourceParams(5, ShadowMap->ShadowResView);
		GRHI->DrawTriangleList(Meshes[i]->GetIndexBuffer());
	}
}

void FSceneRenderer::RenderSceneSkeletalMeshes(FScene* RenderScene, bool bShadowPass)
{
	const vector<FSkeletalMesh*>& SkmMeshes = RenderScene->GetDrawSkeletalMeshes();
	for (size_t i = 0; i < SkmMeshes.size(); i++)
	{
		GRHI->SetVertexAndIndexBuffers(SkmMeshes[i]->GetVertexBuffer(), SkmMeshes[i]->GetIndexBuffer());

		if (!bShadowPass)
		{
			FD3DGraphicPipline* Pso = GRHI->GetPsoObject(SkmMeshes[i]->GetPsoKey());
			GRHI->SetPiplineStateObject(Pso);
		}
		GRHI->SetResourceParams(0, SkmMeshes[i]->MatrixConstantBufferView);
		GRHI->SetResourceParams(1, SkmMeshes[i]->SkeletalConstantBufferView);
		GRHI->SetResourceParams(2, RenderScene->PassViewProj);
		GRHI->SetResourceParams(3, RenderScene->PassLightInfo);
		if (SkmMeshes[i]->GetMaterial()->DiffuseResView)
		{
			GRHI->SetResourceParams(4, SkmMeshes[i]->GetMaterial()->DiffuseResView);
		}
		GRHI->SetResourceParams(5, ShadowMap->ShadowResView);
		GRHI->DrawTriangleList(SkmMeshes[i]->GetIndexBuffer());
	}
}

