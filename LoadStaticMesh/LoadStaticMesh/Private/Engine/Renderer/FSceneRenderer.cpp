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

	MtBufferSingleSize = CalcConstantBufferByteSize(sizeof(FObjectConstants));
	MtBufferTotalSize = MtBufferSingleSize * ObjectNum;

	MatBufferSingleSize = CalcConstantBufferByteSize(sizeof(FMaterialConstants));
	MatBufferTotalSize = MatBufferSingleSize * ObjectNum;

	PassConstantSize = CalcConstantBufferByteSize(sizeof(FPassConstants));
	PassConstantSize = PassConstantSize * PassConNum;

	FCbvSrvDesc BuffersDesc;
	// material buffer count + matrix buffer count + texture count + constant buffer(1)
	BuffersDesc.NeedDesciptorCount = ObjectNum * 2 + TextureMeshNum + PassConNum;
	BuffersDesc.CbMatrix = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX, 0, MtBufferTotalSize, ObjectNum };
	BuffersDesc.CbMaterial = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL, ObjectNum, MatBufferTotalSize , ObjectNum };
	BuffersDesc.CbConstant = { E_CONSTANT_BUFFER_TYPE::TYPE_CB_PASSCONSTANT, ObjectNum * 2, PassConstantSize, PassConNum };
	BuffersDesc.SrvDesc = { ObjectNum * 2 + PassConNum,  TextureMeshNum };
	CreateSrvAndCbvs(BuffersDesc);

	//IMPORTANT: after create pso, will excute first init command queue in the function
	FShader* Vs = CreateShader(L"shader_vs.cso");
	FShader* Ps = CreateShader(L"shader_ps.cso");
	CreatePiplineStateObject(Vs, Ps, SampleAssets::PsoUseTexture, true);

	FShader* VsNoTex = CreateShader(L"shader_vs_notexture.cso");
	FShader* PsNoTex = CreateShader(L"shader_ps_notexture.cso");
	CreatePiplineStateObject(Vs, PsNoTex, SampleAssets::PsoNoTexture);

	delete Vs;
	delete Ps;
	delete VsNoTex;
	delete PsNoTex;

	//in root sig, I put srv behind cbv, so Scene->InitRenderResource must after constant buffer create
	Scene->InitRenderResource();

	PresentToScreen(0, true);
}

void FSceneRenderer::UpdateResource(FScene* Scene)
{
	UpdateMtConstants(Scene);
	UpdateMatConstants(Scene);
	UpdatePassConstants(Scene);
}

void FSceneRenderer::UpdateMtConstants(FScene* Scene)
{
	const std::vector<FMesh*>& Meshes = Scene->GetDrawMeshes();
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATRIX;
	BufferObj->DataSize = MtBufferTotalSize;

	BufferObj->BufferData = new int8_t[MtBufferTotalSize];
	memset(BufferObj->BufferData, 0, MtBufferTotalSize);
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		FObjectConstants ObjConstants;
		XMStoreFloat4x4(&ObjConstants.World, XMMatrixTranspose(Meshes[i]->GetModelMatrix()));
		XMFLOAT4X4 TexMat = Meshes[i]->GetTextureTransform();
		XMStoreFloat4x4(&ObjConstants.TexTransform, XMMatrixTranspose(XMLoadFloat4x4(&TexMat)));
		memcpy(BufferObj->BufferData + i * MtBufferSingleSize, &ObjConstants, sizeof(ObjConstants));
	}

	UpdateConstantBuffer(BufferObj);
	delete BufferObj;
}

void FSceneRenderer::UpdateMatConstants(FScene* Scene)
{
	const std::vector<FMesh*>& Meshes = Scene->GetDrawMeshes();
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_MATERIAL;
	BufferObj->DataSize = MatBufferTotalSize;

	BufferObj->BufferData = new int8_t[MatBufferTotalSize];
	memset(BufferObj->BufferData, 0, MatBufferTotalSize);

	XMFLOAT4X4 Mat;
	for (size_t i = 0; i < Meshes.size(); i++)
	{
		FMaterial* Material = Meshes[i]->GetMaterial();
		Mat = Material->GetMaterialTransform();
		XMMATRIX MatTransform = XMLoadFloat4x4(&Mat);
		FMaterialConstants MatConstants;
		MatConstants.DiffuseAlbedo = Material->GetDiffuseAlbedo();
		MatConstants.FresnelR0 = Material->GetFresnelR0();
		MatConstants.Roughness = Material->GetRoughness();
		XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(MatTransform));

		memcpy(BufferObj->BufferData + i * MatBufferSingleSize, &MatConstants, sizeof(MatConstants));
	}
	UpdateConstantBuffer(BufferObj);
	delete BufferObj;
}

void FSceneRenderer::UpdatePassConstants(FScene* Scene)
{
	const std::vector<FMesh*>& Meshes = Scene->GetDrawMeshes();
	FBufferObject* BufferObj = new FBufferObject();
	BufferObj->Type = E_CONSTANT_BUFFER_TYPE::TYPE_CB_PASSCONSTANT;
	BufferObj->DataSize = PassConstantSize;

	BufferObj->BufferData = new int8_t[PassConstantSize];
	memset(BufferObj->BufferData, 0, PassConstantSize);

	LCamera& Camera = Scene->GetCamera();
	XMFLOAT4X4 MtProj;
	XMStoreFloat4x4(&MtProj, Camera.GetProjectionMatrix());
	XMMATRIX ViewProj = Camera.GetViewMarix() * XMLoadFloat4x4(&MtProj);
	XMStoreFloat4x4(&PassConstant.ViewProj, XMMatrixTranspose(ViewProj));
	PassConstant.EyePosW = Camera.GetCameraLocation();
	PassConstant.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
	PassConstant.Lights[0].Direction = { 0.f, -0.8f, 0.f };
	PassConstant.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	PassConstant.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	PassConstant.Lights[1].Strength = { 0.8f, 0.8f, 0.8f };
	PassConstant.Lights[0].Direction = { 0.0f, -0.707f, -0.707f };
	PassConstant.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	memcpy(BufferObj->BufferData, &PassConstant, sizeof(PassConstant));
	UpdateConstantBuffer(BufferObj);
	delete BufferObj;
}

void FSceneRenderer::EndRenderFram(FScene* Scene)
{
	Scene->EndRender();
}

void FSceneRenderer::Render( FScene* Scene) 
{
	Scene->Render();
}