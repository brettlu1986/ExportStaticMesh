#include "FSceneRenderer.h"
#include "FRHIDef.h"
#include "FScene.h"
#include "FDynamicRHI.h"
#include "FRHI.h"
#include "LAssetDataLoader.h"
#include "FDefine.h"
#include "SampleAssets.h"

FSceneRenderer::FSceneRenderer()
{

}

FSceneRenderer::~FSceneRenderer()
{

}

void FSceneRenderer::RenderInit(FScene* Scene)
{
	UINT ObjectNum = Scene->GetMeshCount();
	UINT TextureMeshNum = Scene->GetMeshWithTextureNum();
	UINT PassConNum = 1;

	UINT MtBufferSize = CalcConstantBufferByteSize(sizeof(FObjectConstants));
	MtBufferSize = MtBufferSize * ObjectNum;

	UINT MatBufferSize = CalcConstantBufferByteSize(sizeof(FMaterialConstants));
	MatBufferSize = MatBufferSize * ObjectNum;

	UINT PassConsBufferSize = CalcConstantBufferByteSize(sizeof(FPassConstants));
	PassConsBufferSize = PassConsBufferSize * PassConNum;

	FCbvSrvDesc BuffersDesc;
	// material buffer count + matrix buffer count + texture count + constant buffer(1)
	BuffersDesc.NeedDesciptorCount = ObjectNum * 2 + TextureMeshNum + PassConNum;
	BuffersDesc.CbMatrix = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX, 0, MtBufferSize, ObjectNum };
	BuffersDesc.CbMaterial = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL, ObjectNum, MatBufferSize , ObjectNum };
	BuffersDesc.CbConstant = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_PASSCONSTANT, ObjectNum * 2, PassConsBufferSize, PassConNum };
	BuffersDesc.SrvDesc = { ObjectNum * 2 + PassConNum,  TextureMeshNum };
	CreateSrvAndCbvs(BuffersDesc);

	//IMPORTANT: after create pso, will excute first init command queue in the function
	FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_ps.cso");
	CreatePiplineStateObject(Vs, Ps, SampleAssets::PsoUseTexture, true);

	FShader* PsNoTex = CreateShader(L"shader_ps_notexture.cso");
	CreatePiplineStateObject(Vs, PsNoTex, SampleAssets::PsoNoTexture);

	delete Vs;
	delete Ps;
	delete PsNoTex;

	//in root sig, I put srv behind cbv, so Scene->InitRenderResource must after constant buffer create
	Scene->InitRenderResource();

	PresentToScreen(0, true);
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render( FScene* Scene) 
{
	Scene->Render();
}