
#include "stdafx.h"
#include "FSceneRenderer.h"
#include "FSkeletalMesh.h"
#include "LCharacter.h"
#include "FShadowMap.h"
#include "LDeviceWindows.h"
#include "LEngine.h"

FSceneRenderer::FSceneRenderer()
{
}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::Initialize(FScene* RenderScene)
{
	//creater use to create rtv, uav, srv
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
	LDeviceWindows* DeviceWindows = dynamic_cast<LDeviceWindows*>(LEngine::GetEngine()->GetPlatformDevice());
	FTextureInitializer Initializer =
	{
		DeviceWindows->GetWidth(), DeviceWindows->GetHeight(), 1, 0, E_GRAPHIC_FORMAT::FORMAT_R24G8_TYPELESS, E_RESOURCE_FLAGS::RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, &ClearValue,
		E_RESOURCE_STATE::RESOURCE_STATE_DEPTH_WRITE
	};
	DsvTex = GRHI->CreateTexture(Initializer);
	DsvView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_DSV, 1, &DsvTex, 0, E_GRAPHIC_FORMAT::FORMAT_D24_UNORM_S8_UINT });

	//create used pipline state objects
	GRHI->CreatePipelineStateObject({ "PsoUseTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), L"ShaderTexVs.cso",
		L"ShaderTexPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });
	GRHI->CreatePipelineStateObject({ "PsoNoTexture", StandardInputElementDescs, _countof(StandardInputElementDescs), L"ShaderVs.cso",
		L"ShaderPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });

	FRHIRasterizerState State;
	State.DepthBias = 25000;
	State.DepthBiasClamp = 0.f;
	State.SlopeScaledDepthBias = 0.1f;
	GRHI->CreatePipelineStateObject({ "ShadowPass", StandardInputElementDescs, _countof(StandardInputElementDescs), L"SampleDepthShaderVs.cso",
		L"SampleDepthShaderPs.cso", 0, FRtvFormat::FORMAT_UNKNOWN, State });
	GRHI->CreatePipelineStateObject({ "SKMPso", SkeletalInputElementDescs, _countof(SkeletalInputElementDescs), L"SkeletalVs.cso",
		L"SkeletalPs.cso", 1, FRtvFormat::FORMAT_R8G8B8A8_UNORM, FRHIRasterizerState() });
	
	//scene static meshes
	const std::vector<FMesh*>& Meshes = RenderScene->GetDrawMeshes();
	for (FMesh* Mesh : Meshes)
	{	
		Mesh->InitRenderResource();
		Mesh->SetPsoKey(Mesh->GetDiffuseTexture() == nullptr ? "PsoNoTexture" : "PsoUseTexture");
	}

	//skeletal meshes
	const std::vector<FSkeletalMesh*>& SkeletalMeshes = RenderScene->GetDrawSkeletalMeshes();
	for(FSkeletalMesh* SkeletalMesh : SkeletalMeshes)
	{
		SkeletalMesh->InitRenderResource();
		SkeletalMesh->SetPsoKey("SKMPso");
	}

	//shadow map
	RenderScene->GetShadowMap()->InitRenderResource();
	RenderScene->GetShadowMap()->SetPsoKey("ShadowPass");

	RenderScene->PassContantView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FPassConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	
	//TODO: Create Scene Resource will move here
	//GRHI->CreateSceneResources(RenderScene);
}

void FSceneRenderer::RenderScene(FScene* RenderScene)
{
	GRHI->BeginRenderScene();

	GRHI->DrawSceneToShadowMap(RenderScene);

	GRHI->RenderSceneObjects(RenderScene);

	// draw skeletal mesh 
	{
		FUserMarker UserMarker("Draw Skeletal Mesh");
		
		std::vector<FResourceHeap*> Heaps; 
		FResourceViewCreater* ResViewCreater = GRHI->GetResourceViewCreater();
		Heaps.push_back(ResViewCreater->GetCbvSrvUavHeap());
		GRHI->SetResourceHeaps(Heaps);

		const std::vector<FSkeletalMesh*>& SkmMeshes = RenderScene->GetDrawSkeletalMeshes();
		for (size_t i = 0; i < SkmMeshes.size(); i++)
		{	
			FD3DGraphicPipline* Pso = GRHI->GetPsoObject(SkmMeshes[i]->GetPsoKey());
			GRHI->SetVertexAndIndexBuffers(SkmMeshes[i]->GetVertexBuffer(), SkmMeshes[i]->GetIndexBuffer());
			GRHI->SetPiplineStateObject(Pso);
			GRHI->SetResourceParams(0, SkmMeshes[i]->MatrixConstantBufferView);
			GRHI->SetResourceParams(1, SkmMeshes[i]->SkeletalConstantBufferView);
			GRHI->SetResourceParams(2, RenderScene->PassContantView);
			GRHI->SetResourceParams(3, SkmMeshes[i]->DiffuseResView);
			GRHI->DrawTriangleList(SkmMeshes[i]->GetIndexBuffer());
		}
	}
	
	GRHI->EndRenderScene();
}

