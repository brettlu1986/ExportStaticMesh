
#include "stdafx.h"
#include "FSceneRenderer.h"
#include "FSkeletalMesh.h"
#include "LCharacter.h"
#include "FShadowMap.h"
#include "LDeviceWindows.h"
#include "LEngine.h"
#include "LLog.h"

FSceneRenderer::FSceneRenderer()
{
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

	DsvTex->Destroy();
	delete DsvTex;
	DsvTex = nullptr;

	delete DsvView;
	DsvView = nullptr;

	if(ShadowMap)
	{
		ShadowMap->Destroy();
		delete ShadowMap;
		ShadowMap = nullptr;
	}
}

void FSceneRenderer::Initialize(FScene* RenderScene)
{
	assert(LEngine::GetEngine()->IsRenderThread());

	//create use to create rtv, uav, srv
	const UINT CbvCount = 50;
	const UINT SrvCount = 100;
	const UINT UavCount = 10;
	const UINT DsvCount = 10;
	const UINT RtvCount = 60;
	const UINT SamplerCount = 20;
	GRHI->CreateResourceViewCreater(CbvCount, SrvCount, UavCount, DsvCount, RtvCount, SamplerCount);

	//create swap render targets
	for(UINT i = 0; i < RENDER_TARGET_COUNT; ++i)
	{
		RenderTargets[i] = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_RTV, 1, nullptr, 0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN, i });
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

	//create used pipline state objects
	GRHI->CreatePipelineStateObject({ "PsoUseTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), L"ShaderTexVs.cso",
		L"ShaderTexPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState(), true });
	GRHI->CreatePipelineStateObject({ "PsoNoTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), L"ShaderVs.cso",
		L"ShaderPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });

	FRHIRasterizerState State;
	State.DepthBias = 25000;
	State.DepthBiasClamp = 0.f;
	State.SlopeScaledDepthBias = 0.1f;
	GRHI->CreatePipelineStateObject({ "ShadowPass", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), L"SampleDepthShaderVs.cso",
		L"SampleDepthShaderPs.cso", 0, FRtvFormat::FORMAT_UNKNOWN, State });

	GRHI->CreatePipelineStateObject({ "SkinShadowPass", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), L"SampleDepthSkinVs.cso",
		L"SampleDepthSkinPs.cso", 0, FRtvFormat::FORMAT_UNKNOWN, State });

	GRHI->CreatePipelineStateObject({ "SKMPso", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), L"SkeletalVs.cso",
		L"SkeletalPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });
	
	ShadowMap = new FShadowMap(SHADOW_WIDTH, SHADOW_HEIGHT);
	ShadowMap->InitRenderResource();
	ShadowMap->SetPsoKey("ShadowPass");

	RenderScene->PassViewProj = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassViewProjection)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	RenderScene->PassLightInfo = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassLightInfo)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	{
		FUserMarker UserMarker("Render Begin");
		vector<FResourceHeap*> Heaps;
		FResourceViewCreater* ResViewCreater = GRHI->GetResourceViewCreater();
		Heaps.push_back(ResViewCreater->GetCbvSrvUavHeap());
		GRHI->SetResourceHeaps(Heaps);
		GRHI->ResourceTransition(RenderTargets[GRHI->GetFrameIndex()], E_RESOURCE_STATE::RESOURCE_STATE_PRESENT, E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET);
	}

	{
		FUserMarker UserMarker("Shadow Pass");
		GRHI->SetViewPortInfo(ShadowMap->ViewPort);
		FD3DGraphicPipline* Pso = GRHI->GetPsoObject(ShadowMap->GetPsoKey());
		GRHI->SetPiplineStateObject(Pso);
		GRHI->ResourceTransition(ShadowMap->ShadowResView, E_RESOURCE_STATE::RESOURCE_STATE_GENERIC_READ, E_RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE);
		GRHI->SetRenderTargets(nullptr, ShadowMap->DsvResView);
		RenderSceneStaticMeshes(RenderScene, false, true);
		RenderSceneStaticMeshes(RenderScene, true, true);

		GRHI->SetPiplineStateObject(GRHI->GetPsoObject("SkinShadowPass"));
		RenderSceneSkeletalMeshes(RenderScene, true);
		GRHI->ResourceTransition(ShadowMap->ShadowResView, E_RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE, E_RESOURCE_STATE::RESOURCE_STATE_GENERIC_READ);
	}
	
	// draw opaque static mesh 
	GRHI->SetViewPortInfo(GRHI->GetDefaultViewPort());
	GRHI->SetRenderTargets(RenderTargets[GRHI->GetFrameIndex()], DsvView);
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
	
	GRHI->ResourceTransition(RenderTargets[GRHI->GetFrameIndex()], E_RESOURCE_STATE::RESOURCE_STATE_RENDER_TARGET, E_RESOURCE_STATE::RESOURCE_STATE_PRESENT);
	
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

