
#include "stdafx.h"

#include "FRHI.h"
#include "FMaterial.h"

FMaterial::FMaterial()
:FRenderResource(E_FRESOURCE_TYPE::F_TYPE_MATERIAL)
, MaterialTransform(MathHelper::Identity4x4())
, DiffuseTex(nullptr)
, NormalTex(nullptr)
, DiffuseResView(nullptr)
, NormalResView(nullptr)
, MaterialConstantBufferView(nullptr)
{

}

FMaterial::FMaterial(E_FRESOURCE_TYPE Type)
:FRenderResource(Type)
, MaterialTransform(MathHelper::Identity4x4())
, DiffuseTex(nullptr)
, NormalTex(nullptr)
, DiffuseResView(nullptr)
, NormalResView(nullptr)
, MaterialConstantBufferView(nullptr)
{

}

FMaterial::~FMaterial()
{

}

void FMaterial::Destroy() 
{
	if (DiffuseTex)
	{
		DiffuseTex->Destroy();
		delete DiffuseTex;
		DiffuseTex = nullptr;

		delete DiffuseResView;
		DiffuseResView = nullptr;
	}

	if (NormalTex)
	{
		NormalTex->Destroy();
		delete NormalTex;
		NormalTex = nullptr;

		delete NormalResView;
		NormalResView = nullptr;
	}

	delete MaterialConstantBufferView;
	MaterialConstantBufferView = nullptr;
}

void FMaterial::Initialize()
{

}

void FMaterial::Init(LMaterial& MaterialData)
{
	DiffuseAlbedo = MaterialData.DiffuseAlbedo;
	FresnelR0 = MaterialData.FresnelR0;
	Roughness = MaterialData.Roughness;
	MaterialTransform = MaterialData.MaterialTransform;

	if(MaterialData.GetDiffuseTexture() != nullptr)
	{
		DiffuseTex = GRHI->CreateTexture(MaterialData.GetDiffuseTexture());
		DiffuseResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &DiffuseTex,
			0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	}

	if (MaterialData.GetNormalTexture() != nullptr)
	{
		NormalTex = GRHI->CreateTexture(MaterialData.GetNormalTexture());
		NormalResView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_SRV, 1, &NormalTex,
			0, E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });
	}

	MaterialConstantBufferView = GRHI->CreateResourceView({ E_RESOURCE_VIEW_TYPE::RESOURCE_VIEW_CBV, 1, nullptr, CalcConstantBufferByteSize(sizeof(FMaterialConstants)), E_GRAPHIC_FORMAT::FORMAT_UNKNOWN });

	UpdateMaterialConstantInRenderThread(MaterialData.DiffuseAlbedo, MaterialData.FresnelR0, MaterialData.Roughness, MaterialData.MaterialTransform);
}

void FMaterial::UpdateMaterialConstantInRenderThread(XMFLOAT4 InDiffuseAlbedo, XMFLOAT3 InFresnelR0, float InRoughness, XMFLOAT4X4 InMatTrans)
{
	DiffuseAlbedo = InDiffuseAlbedo;
	FresnelR0 = InFresnelR0;
	Roughness = InRoughness;
	MaterialTransform = InMatTrans;

	FMaterialConstants MatConstants;
	MatConstants.DiffuseAlbedo = DiffuseAlbedo;
	MatConstants.FresnelR0 = FresnelR0;
	MatConstants.Roughness = Roughness;
	XMStoreFloat4x4(&MatConstants.MatTransform, XMMatrixTranspose(XMLoadFloat4x4(&MaterialTransform)));
	GRHI->UpdateConstantBufferView(MaterialConstantBufferView, &MatConstants);
}
